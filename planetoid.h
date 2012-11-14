#ifndef KERBAL_LAUNCH_PLANETOID_H
#define KERBAL_LAUNCH_PLANETOID_H

#include "vector.h"

typedef struct Planetoid {
    Vector position;
    double radius;
    double gravitational_parameter;
    double rotational_period;
    double atmospheric_attenuation;
} Planetoid;

Planetoid *planetoid_alloc(void);
void planetoid_dealloc(Planetoid *self);
Planetoid *planetoid_init(Planetoid *self);

double planetoid_atm(const Planetoid *self, Vector position);
Vector planetoid_position_radius_vector(const Planetoid *self, Vector position);
double planetoid_position_radius(const Planetoid *self, Vector position);
Vector planetoid_gravitational_force(const Planetoid *self, Vector position);
Vector planetoid_atmospheric_drag(const Planetoid *self, Vector position, Vector speed, double frontal_area, double coeff);

#endif
