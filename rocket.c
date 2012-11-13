#include <stdlib.h>

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
    self->mass = 0.0;

    self->throttle = 1.0;

    self->max_thrust = 200.0;
    self->isp_vac = 375.0;
    self->isp_atm = 325.0;

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
