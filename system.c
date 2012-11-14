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

    self->logging = 0;

    statistics_init(&self->stats);

    return self;
}

void system_run(System *self) {
    //Setup
    system_log_header(self);

    //Run
    while( self->rocket->position.v[0] > 0.0 ) {
        if( system_time(self) > MAX_MISSION_TIME ) {
            self->state = SYSTEM_STATE_ERROR;
            break;
        }
        system_run_one_tick(self);
    }
    self->state = SYSTEM_STATE_SUCCESS;

    //Cleanup
    self->stats.mission_time = system_time(self);
}

void system_run_one_tick(System *self) {
    //Set thrust according to program.
    system_set_throttle(self);

    //Calcualte the mass and mass change.
    double m = self->rocket->mass;
    double dm = rocket_mass_flow(self->rocket, planetoid_atm(self->planetoid, self->rocket->position));
    self->rocket->mass -= dm;

    //Get net force and acceleration.
    Vector f = system_net_force(self);
    Vector a = vector_rect(f.v[0]/m, f.v[1]/m);

    //Move based on the acceleration.
    double dvx = a.v[0]*self->delta_t + self->rocket->velocity.v[0];
    double dvy = a.v[1]*self->delta_t + self->rocket->velocity.v[1];
    Vector delta_v = vector_rect(dvx,dvy);
    double dx = 0.5*a.v[0]*self->delta_t*self->delta_t + self->rocket->velocity.v[0]*self->delta_t + self->rocket->position.v[0];
    double dy = 0.5*a.v[1]*self->delta_t*self->delta_t + self->rocket->velocity.v[1]*self->delta_t + self->rocket->position.v[1];
    Vector delta_r = vector_rect(dx,dy);

    self->rocket->velocity.v[0] += dvx;
    self->rocket->velocity.v[1] += dvy;
    self->rocket->position.v[0] += dx;
    self->rocket->position.v[1] += dy;

    //Then record the statistics.
    system_update_stats(self, delta_r, delta_v);
    system_log_tick(self, delta_r, delta_v);

    //Increment time.
    self->ticks++;
}

double system_time(const System *self) {
    return self->ticks * self->delta_t;
}

Vector system_net_force(const System *self) {
    // Get gravity.
    Vector gravity = planetoid_gravitational_force(self->planetoid, self->rocket->position);

    // Get air resistance.
    Vector air_resistance = planetoid_atmospheric_drag(
        self->planetoid,
        self->rocket->position,
        self->rocket->velocity,
        rocket_frontal_area(self->rocket),
        self->rocket->drag_coefficient
    );

    // Get thrust.
    Vector thrust = rocket_thrust_force(self->rocket, planetoid_atm(self->planetoid, self->rocket->position));

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

    printf("time, m, x, y, vx, vy, r, alt, azm, delta_v, throttle, angle\n");
}

void system_log_tick(const System *self, Vector delta_position, Vector delta_velocity) {
    if(!self->logging)
        return;

    //TODO: Log to a CSV file; header row should be written when run starts.
    if( self->ticks % 100 )
        printf(
            "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
            system_time(self),
            self->rocket->mass,
            VX(self->rocket->position),
            VY(self->rocket->position),
            VX(self->rocket->velocity),
            VY(self->rocket->velocity),
            0.0,
            0.0,
            0.0,
            self->stats.delta_v,
            0.0,
            0.0
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
