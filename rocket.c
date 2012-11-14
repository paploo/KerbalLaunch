#include <stdlib.h>
#include <math.h>

#include "rocket.h"

Rocket *rocket_alloc(void) {
    return (Rocket *)malloc(sizeof(Rocket));
}

void rocket_dealloc(Rocket *self) {
    free(self);
}

Rocket *rocket_init(Rocket *self) {
    for(size_t i=0; i<VECTOR_DIMS; i++) {
        self->position.v[i] = 0.0;
        self->velocity.v[i] = 0.0;
    }
    self->mass = 15.70;
    self->empty_mass = 15.70 - 12.0;

    self->throttle = 1.0;
    self->bearing = M_PI/2.0;

    self->max_thrust = 215.0;
    self->isp_vac = 370.0;
    self->isp_atm = 320.0;

    self->frontal_area = 3.14;
    self->drag_coefficient = 0.001;

    return self;
}

double rocket_isp(const Rocket *self, double atm) {
    return atm*self->isp_atm + (1.0-atm)*self->isp_vac;
}

double rocket_mass_flow(const Rocket *self, double atm) {
    return rocket_thrust(self, atm) / (rocket_isp(self, atm) * ISP_SURFACE_GRAVITY);
}

double rocket_thrust(const Rocket *self, double atm) {
    return (self->throttle * self->max_thrust);
}

Vector rocket_thrust_force(const Rocket *self, double atm) {
    //If out of fuel, no thrust.
    if(self->mass <= self->empty_mass)
        return vector();
    //Claculate thrust force.
    double f_mag = rocket_thrust(self, atm);
    double f_azm = vector_azm(self->velocity);
    return vector_polar(f_mag, f_azm);
}

double rocket_frontal_area(const Rocket *self) {
    //TODO: Change so that we smoothly change from the frontal area to the side_area depending on velocity vector angle.
    return self->frontal_area;
}
