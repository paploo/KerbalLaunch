#ifndef KERBAL_LAUNCH_ROCKET_H
#define KERBAL_LAUNCH_ROCKET_H

#define ISP_SURFACE_GRAVITY 9.8072

typedef struct Rocket {
    double position[3];
    double velocity[3];
    double mass;

    double throttle;

    double max_thrust;
    double isp_vac;
    double isp_atm;
} Rocket;


Rocket *rocket_alloc(void);
void rocket_dealloc(Rocket *self);
Rocket *rocket_init(Rocket *self);

double rocket_isp(Rocket *self, double atm);
double rocket_thrust(Rocket *self, double atm);
double rocket_mass_flow(Rocket *self, double atm);

#endif
