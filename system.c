#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "system.h"

System *system_alloc(void) {
    return (System *)malloc(sizeof(System));
}

void system_dealloc(System *self) {
    free(self);
}

System *system_init(System *self) {
    self->rocket = NULL;
    self->planetoid = NULL;
    self->throttle_program = NULL;
    self->bearing_program = NULL;

    self->delta_t = 1.0/100.0;

    self->state = SYSTEM_STATE_READY;
    self->ticks = 0;

    self->logging = false;

    statistics_init(&self->stats);

    return self;
}

void system_run(System *self) {
    //Setup
    system_log_header(self);

    //Run
    assert(self->state == SYSTEM_STATE_READY);
    self->state = SYSTEM_STATE_RUNNING;
    double altitude = planetoid_position_radius(self->planetoid, self->rocket->position) - self->planetoid->radius;

    while( altitude >= 0.0 ) {
        if( system_time(self) > MAX_MISSION_TIME ) {
            self->state = SYSTEM_STATE_ERROR;
            break;
        }
        system_run_one_tick(self);
        altitude = planetoid_position_radius(self->planetoid, self->rocket->position) - self->planetoid->radius;
    }

    if(self->state >= 0)
        self->state = SYSTEM_STATE_SUCCESS;

    //Cleanup
    self->stats.mission_time = system_time(self);
}

void system_run_one_tick(System *self) {
    double delta_t = self->delta_t;
#ifdef DEBUG
    printf("n: %lu, t: %f, dt: %f\n", self->ticks, system_time(self), delta_t);
#endif

    //Set thrust according to program.
    system_set_throttle(self);

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

#ifdef DEBUG
    printf("dm: %f, dv: (%f, %f), dr: (%f, %f)\n", dm, dvx, dvy, dx, dy);
    printf("m: %f, v: (%f, %f), r: (%f, %f)\n", m, VX(self->rocket->velocity), VY(self->rocket->velocity), VX(self->rocket->position), VY(self->rocket->position));
#endif

    // Now apply the changes.
    self->rocket->mass -= dm;
    self->rocket->velocity.v[0] += dvx;
    self->rocket->velocity.v[1] += dvy;
    self->rocket->position.v[0] += dx;
    self->rocket->position.v[1] += dy;

    //Then record the statistics.
    system_update_stats(self, delta_r, delta_v);
    system_log_tick(self, dm, f, a, delta_r, delta_v);

    //Increment time.
    self->ticks++;
}

double system_time(const System *self) {
    return self->ticks * self->delta_t;
}

Vector system_net_force(const System *self) {
    // Get gravity.
    Vector gravity = planetoid_gravitational_force(self->planetoid, self->rocket->mass, self->rocket->position);

    // Get air resistance.
    Vector air_resistance = planetoid_atmospheric_drag(
        self->planetoid,
        self->rocket->position,
        self->rocket->velocity,
        rocket_drag(self->rocket),
        self->rocket->mass
    );

    // Get thrust.
    Vector thrust = rocket_thrust_force(self->rocket, planetoid_atm(self->planetoid, self->rocket->position));

#ifdef DEBUG
    printf("gravity:(%f, %f); thrust:(%f, %f), air:(%f, %f)\n", VX(gravity), VY(gravity), VX(thrust), VY(thrust), VX(air_resistance), VY(air_resistance));
#endif

    // Sum.
    double fx = VX(gravity) + VX(air_resistance) + VX(thrust);
    double fy = VY(gravity) + VY(air_resistance) + VY(thrust);
    return vector_rect(fx, fy);
}

void system_update_stats(System *self, Vector delta_position, Vector delta_velocity) {
    self->stats.distance_travelled += vector_mag(delta_position);
    self->stats.delta_v += vector_mag(delta_velocity);

    double radius = planetoid_position_radius(self->planetoid, self->rocket->position);
    if(radius > self->stats.max_radius) {
        self->stats.max_radius = radius;
        self->stats.max_radius_time = system_time(self);
    }
}

void system_log_header(const System *self) {
    if(!self->logging)
        return;

    printf("tick, time, m, dm, x, y, vx, vy, r, alt, azm, delta_v, fx, fy, throttle, bearing\n");
}

void system_log_tick(const System *self, double delta_mass, Vector force, Vector acceleration, Vector delta_position, Vector delta_velocity) {
    if(!self->logging)
        return;

    //TODO: Log to a CSV file; header row should be written when run starts.
    if( (self->ticks % 100) == 0 )
        printf(
            "%lu, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
            self->ticks,
            system_time(self),
            self->rocket->mass,
            delta_mass,
            VX(self->rocket->position),
            VY(self->rocket->position),
            VX(self->rocket->velocity),
            VY(self->rocket->velocity),
            planetoid_position_radius(self->planetoid, self->rocket->position),
            planetoid_position_radius(self->planetoid, self->rocket->position) - self->planetoid->radius,
            -1.0,
            self->stats.delta_v,
            VX(force),
            VY(force),
            self->rocket->throttle,
            self->rocket->bearing
        );
}

void system_set_throttle(System *self) {
    double altitude = planetoid_position_radius(self->planetoid, self->rocket->position) - self->planetoid->radius;

    int error=0;
    double throttle = program_lookup(self->throttle_program, altitude, &error);
    assert(error==0);

    self->rocket->throttle = throttle;
}

void system_set_bearing(System *self) {
    double altitude = planetoid_position_radius(self->planetoid, self->rocket->position) - self->planetoid->radius;

    int error=0;
    double bearing = program_lookup(self->bearing_program, altitude, &error);
    assert(error==0);

    self->rocket->bearing = bearing;
}
