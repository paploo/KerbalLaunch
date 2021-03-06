#include <stdlib.h>
#include <assert.h>

#include "system.h"

System *system_alloc(void) {
    return (System *)malloc(sizeof(System));
}

void system_dealloc(System *self) {
    free(self);
}

System *system_init(System *self) {
    statistics_init(&self->stats);

    self->rocket = NULL;
    self->planetoid = NULL;

    self->throttle_program = NULL;
    self->altitude_angle_program = NULL;
    self->throttle_cutoff_radius = -1.0;

    self->delta_t = 1.0/SYSTEM_TICKS_PER_SECOND;

    self->state = SYSTEM_STATE_READY;
    self->ticks = 0;
    self->frame = NULL;

    self->collect_stats = false;

    self->logging = false;
    self->log = stdout;

    return self;
}

void system_run(System *self) {
    //Sanity check
    assert(self->rocket);
    assert(self->planetoid);
    assert(self->throttle_program);
    assert(self->altitude_angle_program);
    assert(self->state == SYSTEM_STATE_READY);

    //Setup
    system_log_header(self);
    Frame frame;
    frame_init(&frame);
    self->frame = &frame;

    //Run
    self->state = SYSTEM_STATE_RUNNING;
    double altitude = planetoid_position_altitude(self->planetoid, self->rocket->position);
    double radial_velocity = planetoid_radial_velocity(self->planetoid, self->rocket->position, self->rocket->velocity);

    //We have the radial velocity cutoff a little below 0.0, because high tick rates with float precision can cause this to abort early.
    while( altitude >= 0.0 && radial_velocity >= -0.0001 ) {
        if( system_time(self) > SYSTEM_MAX_MISSION_TIME ) {
            self->state = SYSTEM_STATE_ERROR;
            break;
        }
        system_run_one_tick(self);
        altitude = planetoid_position_altitude(self->planetoid, self->rocket->position);
        radial_velocity = planetoid_radial_velocity(self->planetoid, self->rocket->position, self->rocket->velocity);
    }

    //If we didn't collect stats, we take the last frame for the stats as it was at apex.
    self->stats.frame = frame;

    //Cleanup
    if(self->state >= 0)
        self->state = SYSTEM_STATE_SUCCESS;
    self->frame = NULL;
}

void system_run_one_tick(System *self) {
    //Allocate the frame on the stack and point to it.

    //Got tired of writing self->delta_t.
    double delta_t = self->delta_t;

    //Set rocket according to program.
    system_set_throttle(self);
    system_set_altitude_angle(self);

    //Calcualte the mass and mass change.
    double m = self->rocket->mass;
    double dm = rocket_mass_flow(self->rocket, planetoid_atm(self->planetoid, self->rocket->position)) * delta_t;

    //Get net force and acceleration.
    Vector f = system_net_force(self);
    Vector a = vector_rect(f.v[0]/m, f.v[1]/m);

    //Move based on the acceleration.
    double dvx = a.v[0]*delta_t;
    double dvy = a.v[1]*delta_t;
    Vector delta_v = vector_rect(dvx,dvy);
    double dx = 0.5*a.v[0]*delta_t*delta_t + self->rocket->velocity.v[0]*delta_t;
    double dy = 0.5*a.v[1]*delta_t*delta_t + self->rocket->velocity.v[1]*delta_t;
    Vector delta_r = vector_rect(dx,dy);

    //Set the parts of the frame that didn't come from other places.
    self->frame->ticks = self->ticks;
    self->frame->time = system_time(self);
    self->frame->mass = m;
    self->frame->position = self->rocket->position;
    self->frame->velocity = self->rocket->velocity;
    self->frame->delta_t = delta_t;
    self->frame->delta_mass = dm;
    self->frame->delta_position = delta_r;
    self->frame->delta_velocity = delta_v;
    self->frame->radius = planetoid_position_radius(self->planetoid, self->frame->position);
    self->frame->altitude = planetoid_position_altitude(self->planetoid, self->frame->position);
    self->frame->azimuth = planetoid_position_azimuth(self->planetoid, self->frame->position);
    self->frame->energy = system_energy(self);
    self->frame->angular_momentum = system_angular_momentum(self);
    self->frame->rocket_remaining_fuel_mass = self->rocket->mass - self->rocket->empty_mass;
    self->frame->rocket_remaining_ideal_delta_v = rocket_ideal_delta_v(self->rocket);

    //Then record the rame statistics.
    system_update_stats(self);
    system_log_tick(self);

    // Now apply the changes just before cleanup.
    self->rocket->mass -= dm;
    self->rocket->velocity.v[0] += dvx;
    self->rocket->velocity.v[1] += dvy;
    self->rocket->position.v[0] += dx;
    self->rocket->position.v[1] += dy;
    self->ticks++;
}


double rocket_remaining_ideal_delta_v;
double system_time(const System *self) {
    return self->ticks * self->delta_t;
}

Vector system_net_force(const System *self) {
    // Get gravity.
    Vector gravity = planetoid_gravitational_force(
        self->planetoid,
        self->rocket->mass,
        self->rocket->position
    );

    // Get air resistance.
    Vector drag = planetoid_atmospheric_drag(
        self->planetoid,
        self->rocket->position,
        self->rocket->velocity,
        rocket_drag(self->rocket),
        self->rocket->mass
    );

    // Get thrust.
    Vector thrust = rocket_thrust_force(
        self->rocket,
        planetoid_atm(self->planetoid, self->rocket->position),
        planetoid_position_azimuth(self->planetoid, self->rocket->position)
    );

    // Sum.
    double fx = VX(gravity) + VX(drag) + VX(thrust);
    double fy = VY(gravity) + VY(drag) + VY(thrust);
    Vector f = vector_rect(fx, fy);

    // Put in frame.
    self->frame->force = f;
    self->frame->force_thrust = thrust;
    self->frame->force_gravity = gravity;
    self->frame->force_drag = drag;

    // Return;;
    return vector_rect(fx, fy);
}

void system_set_throttle(System *self) {
    double throttle;

    // If a throttle cutoff is set, consider if the current apoapsis is at or above that altitude.
    bool consider_cutoff = self->throttle_cutoff_radius > 0.0;

    // Get the apoapsis/periapsis, and cache in the frame.
    double periapsis, apoapsis;
    bool closed = system_apses(self, &periapsis, &apoapsis);
    self->frame->closed_orbit = closed;
    self->frame->apoapsis = apoapsis;
    self->frame->periapsis = periapsis;

    // Calculate the throttle.
    if(consider_cutoff && (!closed || (apoapsis >= self->throttle_cutoff_radius))) {
        throttle = 0.0;
    } else {
        double altitude = planetoid_position_altitude(self->planetoid, self->rocket->position);
        int error=0;
        throttle = program_lookup(self->throttle_program, altitude, &error);
        assert(error==0);
    }

    // Set the throttle.
    self->rocket->throttle = throttle;
    self->frame->throttle = throttle;
}

void system_set_altitude_angle(System *self) {
    double altitude = planetoid_position_radius(self->planetoid, self->rocket->position) - self->planetoid->radius;

    int error=0;
    double altitude_angle = program_lookup(self->altitude_angle_program, altitude, &error);
    assert(error==0);

    self->rocket->altitude_angle = altitude_angle;
    self->frame->altitude_angle = altitude_angle;
}

bool system_apses(const System *self, double *periapsis, double *apoapsis) {
    double angular_momentum = system_angular_momentum(self);
    double energy =  system_energy(self);
    double gravitational_parameter = self->planetoid->gravitational_parameter;

    return orbit_apses(gravitational_parameter, angular_momentum, energy, periapsis, apoapsis);
}

double system_energy(const System *self) {
    return rocket_kinetic_energy(self->rocket) + planetoid_potential_energy(self->planetoid, self->rocket->position);
}

double system_angular_momentum(const System *self) {
    return planetoid_angular_momentum(self->planetoid, self->rocket->velocity, self->rocket->position);
}

void system_update_stats(System *self) {
    if(!self->collect_stats)
        return;

    /* The stats at apoapsis are really what is interesting. */
    if(self->frame->radius > self->stats.frame.radius) {
        self->stats.frame = *(self->frame);

        self->stats.distance_travelled += vector_mag(self->frame->delta_position);

        double alpha = self->frame->delta_t / self->frame->mass;
        self->stats.delta_v_thrust += alpha * vector_mag(self->frame->force_thrust);
        self->stats.delta_v_drag += alpha * vector_mag(self->frame->force_drag);
        self->stats.delta_v_gravity += alpha * vector_mag(self->frame->force_gravity);

        double dr = vector_mag(self->frame->delta_position);
        self->stats.work_thrust += dr * vector_mag(self->frame->force_thrust);
        self->stats.work_drag += dr * vector_mag(self->frame->force_drag);
        self->stats.work_gravity += dr * vector_mag(self->frame->force_gravity);
    }
}

void system_log_header(const System *self) {
    if(!self->logging)
        return;

    fprintf(self->log, "tick, time, m, dm, x, y, vx, vy, r, alt, azm, fx, fy, throttle, altitude_angle\n");
}

void system_log_tick(const System *self) {
    if(!self->logging)
        return;

    //TODO: Log to a CSV file; header row should be written when run starts.
    if( (self->ticks % (SYSTEM_LOG_INTERVAL_SECONDS*SYSTEM_TICKS_PER_SECOND)) == 0 )
        fprintf(
            self->log,
            "%lu, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
            self->frame->ticks,
            self->frame->time,
            self->frame->mass,
            self->frame->delta_mass,
            VX(self->frame->position),
            VY(self->frame->position),
            VX(self->frame->velocity),
            VY(self->frame->velocity),
            self->frame->radius,
            self->frame->altitude,
            self->frame->azimuth,
            VX(self->frame->force),
            VY(self->frame->force),
            self->frame->throttle,
            self->frame->altitude_angle
        );
}

/*
 * For closed orbits, the periapsis and apoapsis are as expected.
 * For open orbits, the apoapsis is a negative value that approaches zero as the exces velocity increases.
 * For hyperbolic orbits, the apoapsis is infinite.
 */
bool orbit_apses(double gravitational_parameter, double angular_momentum, double energy, double *periapsis, double *apoapsis) {
    if(energy == 0.0) {
        *apoapsis = INFINITY;
        *periapsis = angular_momentum*angular_momentum / (2.0*gravitational_parameter);
        return false;
    }

    double eccentricity = orbit_eccentricity(gravitational_parameter, angular_momentum, energy);
    double semimajor_axis = -(gravitational_parameter)/(2.0*energy);

    *periapsis = semimajor_axis * (1.0 - eccentricity);
    *apoapsis = semimajor_axis * (1.0 + eccentricity);

    return (energy > 0.0) ? false : true;
}

double orbit_eccentricity(double gravitational_parameter, double angular_momentum, double energy) {
    double numerator = 2.0 * angular_momentum * angular_momentum * energy;
    double denominator = gravitational_parameter * gravitational_parameter;

    double radicand = 1.0 + numerator/denominator;
    // Sometimes rounding errors on near circular orbits make the radicand negative when it should be near zero.
    if(radicand < 0.0)
        radicand = 0.0;

    return sqrt(radicand);
}
