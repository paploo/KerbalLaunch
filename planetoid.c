#include <stdlib.h>
#include <math.h>

#include "planetoid.h"

Planetoid *planetoid_alloc(void) {
    return (Planetoid *)malloc(sizeof(Planetoid));
}

void planetoid_dealloc(Planetoid *self) {
    free(self);
}

Planetoid *planetoid_init(Planetoid *self) {
    for(size_t i=0; i<VECTOR_DIMS; i++)
        self->position.v[i] = 0.0;

    self->radius = 600000.0;
    self->gravitational_parameter = 3531600000000.0;
    self->rotational_period = 21600.0;

    self->atmospheric_attenuation = 5000.0;

    return self;
}

Vector planetoid_gravitational_force(Planetoid *self, Vector position) {
    // Compute the relative position vector, r
    Vector r_vec = vector_sub(position, self->position);

    // Compute the gravitational force magnitude
    double f_mag = -(self->gravitational_parameter)/vector_mag(r_vec);

    // Compute the gravitational force angle
    double f_ang = M_PI/2.0; //TODO: Compute using the angle of r_vec.

    Vector force;
    force.v[0] = f_mag * cos(f_ang);
    force.v[1] = f_mag * sin(f_ang);
    return force;
}

Vector planetoid_atmospheric_drag(Planetoid *self, Vector position, Vector speed, double frontal_area, double coeff) {
    //TODO: Implement the atmospheric drag method.
    //Vector r_vec = vector_sub(psotion, self->position);
    //double r = -(self->
    //double f_mag = 0.5 * planetoid_atm(r)
    Vector r;
    return r;
}
