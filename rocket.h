#ifndef KERBAL_LAUNCH_ROCKET_H
#define KERBAL_LAUNCH_ROCKET_H

#define ISP_SURFACE_GRAVITY 9.8072

#include "vector.h"

typedef struct Rocket {
    Vector position;
    Vector velocity;
    double mass;

    double throttle;

    double max_thrust;
    double isp_vac;
    double isp_atm;
} Rocket;


Rocket *rocket_alloc(void);
void rocket_dealloc(Rocket *self);
Rocket *rocket_init(Rocket *self);

double rocket_isp(const Rocket *self, double atm);
double rocket_thrust(const Rocket *self, double atm);
double rocket_mass_flow(const Rocket *self, double atm);

#endif
