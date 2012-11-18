#ifndef KERBAL_LAUNCH_ROCKET_H
#define KERBAL_LAUNCH_ROCKET_H

#define ISP_SURFACE_GRAVITY 9.8072

#include "vector.h"

typedef struct Rocket {
    Vector position;
    Vector velocity;
    double mass;

    double throttle;
    double altitude_angle;

    double max_thrust;
    double isp_vac;
    double isp_atm;

    double empty_mass;

    double max_drag;
} Rocket;

Rocket *rocket_alloc(void);
void rocket_dealloc(Rocket *self);
Rocket *rocket_init(Rocket *self);

double rocket_isp(const Rocket *self, double atm);
double rocket_thrust(const Rocket *self, double atm);
Vector rocket_thrust_force(const Rocket *self, double atm, double azm);
double rocket_mass_flow(const Rocket *self, double atm);
double rocket_drag(const Rocket *self);

double rocket_momentum(const Rocket *self);
double rocket_kinetic_energy(const Rocket *self);

// At the current fuel level, in a vacuum, returns the delta_v that this rocket
// could produce according to the Tsiolkovsky rocket equation.
double rocket_ideal_delta_v(const Rocket *self);

#endif
