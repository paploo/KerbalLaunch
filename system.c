#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "system.h"

#define SYSTEM_TICKS_PER_SECOND 1000
#define LOG_INTERVAL_SECONDS 1

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

    self->delta_t = 1.0/SYSTEM_TICKS_PER_SECOND;

    self->state = SYSTEM_STATE_READY;
    self->ticks = 0;
    self->frame = NULL;

    self->logging = false;

    return self;
}

void system_run(System *self) {
    //Sanity check
    assert(self->rocket);
    assert(self->planetoid);
    assert(self->throttle_program);
    assert(self->altitude_angle_program);

    //Setup
    system_log_header(self);

    //Run
    assert(self->state == SYSTEM_STATE_READY);
    self->state = SYSTEM_STATE_RUNNING;
    double altitude = planetoid_position_radius(self->planetoid, self->rocket->position) - self->planetoid->radius;
    double radial_velocity = self->rocket->velocity.v[0]; //TODO: This needs to calculate the radial velocity in relation to the planet, not the y velocity!

    while( altitude >= 0.0 && radial_velocity >= 0.0 ) {
        if( system_time(self) > MAX_MISSION_TIME ) {
            self->state = SYSTEM_STATE_ERROR;
            break;
        }
        system_run_one_tick(self);
        altitude = planetoid_position_radius(self->planetoid, self->rocket->position) - self->planetoid->radius;
        radial_velocity = self->rocket->velocity.v[0];
    }

    if(self->state >= 0)
        self->state = SYSTEM_STATE_SUCCESS;

    //Cleanup
    self->stats.mission_time = system_time(self);
}

void system_run_one_tick(System *self) {
    //Allocate the frame on the stack and point to it.
    Frame frame;
    self->frame = &frame;

    //Got tired of writing self->delta_t.
    double delta_t = self->delta_t;

#ifdef DEBUG
    printf("n: %lu, t: %f, dt: %f\n", self->ticks, system_time(self), delta_t);
#endif

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

    //Set the frame.
    frame.ticks = self->ticks;
    frame.t = system_time(self);
    frame.mass = m;
    frame.position = self->rocket->position;
    frame.velocity = self->rocket->velocity;
    frame.delta_t = delta_t;
    frame.delta_mass = dm;
    frame.delta_position = delta_r;
    frame.delta_velocity = delta_v;

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
    system_update_stats(self);
    system_log_tick(self);

    //Cleanup
    self->frame = NULL;

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
    Vector drag = planetoid_atmospheric_drag(
        self->planetoid,
        self->rocket->position,
        self->rocket->velocity,
        rocket_drag(self->rocket),
        self->rocket->mass
    );

    // Get thrust.
    Vector thrust = rocket_thrust_force(self->rocket, planetoid_atm(self->planetoid, self->rocket->position));

#ifdef DEBUG
    printf("gravity:(%f, %f); thrust:(%f, %f), air:(%f, %f)\n", VX(gravity), VY(gravity), VX(thrust), VY(thrust), VX(drag), VY(drag));
#endif

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

void system_update_stats(System *self) {
    self->stats.distance_travelled += vector_mag(self->frame->delta_position);
    self->stats.delta_v += vector_mag(self->frame->delta_velocity);

    double radius = planetoid_position_radius(self->planetoid, self->frame->position);
    if(radius > self->stats.max_radius) {
        self->stats.max_radius = radius;
        self->stats.max_radius_time = self->frame->t;
    }
}

void system_log_header(const System *self) {
    if(!self->logging)
        return;

    printf("tick, time, m, dm, x, y, vx, vy, r, alt, azm, fx, fy, throttle, altitude_angle\n");
}

void system_log_tick(const System *self) {
    if(!self->logging)
        return;

    //TODO: Log to a CSV file; header row should be written when run starts.
    if( (self->ticks % (LOG_INTERVAL_SECONDS*SYSTEM_TICKS_PER_SECOND)) == 0 )
        printf(
            "%lu, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
            self->frame->ticks,
            self->frame->t,
            self->frame->mass,
            self->frame->delta_mass,
            VX(self->frame->position),
            VY(self->frame->position),
            VX(self->frame->velocity),
            VY(self->frame->velocity),
            planetoid_position_radius(self->planetoid, self->frame->position),
            planetoid_position_radius(self->planetoid, self->frame->position) - self->planetoid->radius,
            -1.0,
            VX(self->frame->force),
            VY(self->frame->force),
            self->frame->throttle,
            self->frame->altitude_angle
        );
}

void system_set_throttle(System *self) {
    double altitude = planetoid_position_radius(self->planetoid, self->rocket->position) - self->planetoid->radius;

    int error=0;
    double throttle = program_lookup(self->throttle_program, altitude, &error);
    assert(error==0);

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
