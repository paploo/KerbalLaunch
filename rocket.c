#include <stdlib.h>

#include "rocket.h"

Rocket *rocket_alloc(void) {
    return (Rocket *)malloc(sizeof(Rocket));
}

void rocket_dealloc(Rocket *self) {
    free(self);
}

Rocket *rocket_init(Rocket *self) {
    for(int i=0; i<3; i++) {
        self->position[i] = 0.0;
        self->velocity[i] = 0.0;
    }
    self->mass = 0.0;

    self->throttle = 1.0;

    self->max_thrust = 200.0;
    self->isp_vac = 375.0;
    self->isp_atm = 325.0;

    return self;
}

double rocket_isp(Rocket *self, double atm) {
    return atm*self->isp_atm + (1.0-atm)*self->isp_vac;
}

double rocket_mass_flow(Rocket *self, double atm) {
    return rocket_thrust(self, atm) / (rocket_isp(self, atm) * ISP_SURFACE_GRAVITY);
}

double rocket_thrust(Rocket *self, double atm) {
    return (self->throttle * self->max_thrust);
}
