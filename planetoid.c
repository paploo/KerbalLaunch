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

double planetoid_atm(Planetoid *self, double radius) {
    return exp(-radius / self->atmospheric_attenuation);
}

Vector planetoid_gravitational_force(Planetoid *self, Vector position) {
    // Compute the relative position vector, r
    Vector r_vec = vector_sub(position, self->position);

    // Compute the gravitational force magnitude
    double f_mag = -(self->gravitational_parameter)/vector_mag(r_vec);

    // Compute the gravitational force angle
    double f_azm = M_PI/2.0; //TODO: Compute using the angle of r_vec.

    // Return the vector.
    return vector_polar(f_mag, f_azm);
}

Vector planetoid_atmospheric_drag(Planetoid *self, Vector position, Vector velocity, double frontal_area, double coeff) {
    Vector r_vec = vector_sub(position, self->position);
    double r = vector_mag(r_vec);

    double v = vector_mag(velocity);

    double f_mag = 0.5 * planetoid_atm(self, r) * coeff * frontal_area * v * v;
    double f_azm = vector_azm(velocity);

    return vector_polar(f_mag, f_azm);
}
