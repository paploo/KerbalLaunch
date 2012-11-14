#include <stdlib.h>

#include "system.h"

System *system_alloc(void) {
    return (System *)malloc(sizeof(System));
}

void system_dealloc(System *self) {
    free(self);
}

System *system_init(System *self) {
    self->rocket = NULL;
    self->throttle_program = NULL;
    self->planetoid = NULL;

    self->delta_t = 1.0/100.0;

    self->state = SYSTEM_STATE_READY;
    self->time = 0.0;
    statistics_init(&self->stats);

    return self;
}

void system_run(System *self) {
    while( self->rocket->position.v[0] > 0.0 ) {
        if( self->time > MAX_MISSION_TIME ) {
            self->state = SYSTEM_STATE_ERROR;
            break;
        }
        system_run_one_tick(self);
    }
    self->state = SYSTEM_STATE_SUCCESS;
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
    double dx = 0.5*a.v[0]*self->delta_t*self->delta_t + self->rocket->velocity.v[0]*self->delta_t + self->rocket->position.v[0];
    double dy = 0.5*a.v[1]*self->delta_t*self->delta_t + self->rocket->velocity.v[1]*self->delta_t + self->rocket->position.v[1];

    self->rocket->velocity.v[0] += dvx;
    self->rocket->velocity.v[1] += dvy;
    self->rocket->position.v[0] += dx;
    self->rocket->position.v[1] += dy;

    //Then record the statistics.
    system_update_stats(self, vector_rect(dx,dy), vector_rect(dvx,dvy));

    //Increment time.
    self->time += self->delta_t;

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

    double radius = planetoid_position_radius(self->planetoid, self->rocket->position);
    if(radius > self->stats.max_radius) {
        self->stats.max_radius = radius;
        self->stats.max_radius_time = self->time;
    }
}

void system_set_throttle(System *self) {
    //TODO: Set Thrust according to program!
    self->rocket->throttle = 1.0;
}
