#include <stdlib.h>

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
    /* TODO: This really should calculate the XY force component, not the mag. */
    Vector r_vec = vector_sub(position, self->position);
    double r = -(self->gravitational_parameter)/vector_mag(r_vec);

    Vector force;
    force.v[0] = 0.0;
    force.v[1] = r;
    return force;
}
