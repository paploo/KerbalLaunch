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
    self->altitude_angle = M_PI/2.0;

    self->max_thrust = 215.0;
    self->isp_vac = 370.0;
    self->isp_atm = 320.0;

    self->max_drag = 0.20;

    return self;
}

double rocket_isp(const Rocket *self, double atm) {
    return atm*self->isp_atm + (1.0-atm)*self->isp_vac;
}

double rocket_mass_flow(const Rocket *self, double atm) {
    return rocket_thrust(self, atm) / (rocket_isp(self, atm) * ISP_SURFACE_GRAVITY);
}

double rocket_thrust(const Rocket *self, double atm) {
    //If out of fuel, no thrust.
    if(self->mass <= self->empty_mass)
        return 0.0;
    else
        return self->throttle * self->max_thrust;
}

Vector rocket_thrust_force(const Rocket *self, double atm, double azm) {
    //Calculate thrust force.
    double f_mag = rocket_thrust(self, atm);

    //The azm is the azimuth angle of the vector from the center of the planetoid to the rocket.
    //The zenith angle is angle of the ship relative to the zenith angle.
    //The altitude angle is the complement of the zenith angle.
    //To match KSP intuition, we thus define altitude_angle such that:
    //  0.0 degrees points in the positive-x direction of the local horizon frame.
    // 90.0 degrees points straight-up.
    double f_azm = azm - M_PI/2.0 + self->altitude_angle;

    // Return.
    return vector_polar(f_mag, f_azm);
}

double rocket_drag(const Rocket *self) {
    //NOTE: In the future this should be different for the side, but KSP 0.17 doesn't seem to care.
    return self->max_drag;
}

double rocket_momentum(const Rocket *self) {
    return self->mass * vector_mag(self->velocity);
}

double rocket_kinetic_energy(const Rocket *self) {
    double v = vector_mag(self->velocity);
    return 0.5 * v * v;
}
