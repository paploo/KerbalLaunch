#ifndef KERBAL_LAUNCH_PLANETOID_H
#define KERBAL_LAUNCH_PLANETOID_H

#include "vector.h"

typedef struct Planetoid {
    Vector position;
    double radius;
    double gravitational_parameter;
    double rotational_period;
    double atmospheric_attenuation;
    double max_atmospheric_altitude;
} Planetoid;

Planetoid *planetoid_alloc(void);
void planetoid_dealloc(Planetoid *self);
Planetoid *planetoid_init(Planetoid *self);

double planetoid_atm(const Planetoid *self, Vector position);
double planetoid_rho(const Planetoid *self, Vector position);

Vector planetoid_relative_position(const Planetoid *self, Vector position);
double planetoid_position_altitude(const Planetoid *self, Vector position);
double planetoid_position_radius(const Planetoid *self, Vector position);
double planetoid_position_azimuth(const Planetoid *self, Vector position);

Vector planetoid_gravitational_force(const Planetoid *self, double mass, Vector position);
Vector planetoid_atmospheric_drag(const Planetoid *self, Vector position, Vector velocity, double max_drag, double mass);
Vector planetoid_irl_atmospheric_drag(const Planetoid *self, Vector position, Vector speed, double frontal_area, double coeff);

#endif
