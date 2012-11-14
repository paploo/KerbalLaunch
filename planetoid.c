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

double planetoid_atm(const Planetoid *self, Vector position) {
    double r = planetoid_position_radius(self, position);
    double a = r - self->radius;
    if( a >= 0 )
        return exp(-r / self->atmospheric_attenuation);
    else
        return 1.0;
}

Vector planetoid_position_radius_vector(const Planetoid *self, Vector position) {
    return vector_sub(position, self->position);
}

double planetoid_position_radius(const Planetoid *self, Vector position) {
    return vector_mag( planetoid_position_radius_vector(self, position) );
}

Vector planetoid_gravitational_force(const Planetoid *self, Vector position) {
    //Compute the relative position vector, r
    Vector r_vec = planetoid_position_radius_vector(self, position);

    //Compute the gravitational force magnitude. Note that it is negative because it goes opposite the position.
    double f_mag = -(self->gravitational_parameter)/vector_mag(r_vec);

    //Compute the gravitational force angle. We take into account it's oppositeness with the negative magnitude.
    double f_azm = vector_azm(r_vec);

    //Return the vector.
    return vector_polar(f_mag, f_azm);
}

Vector planetoid_atmospheric_drag(const Planetoid *self, Vector position, Vector velocity, double frontal_area, double coeff) {
    double v = vector_mag(velocity);

    //Calculate the magnitude.  Note that it is negative because it goes opposite the velocity.
    double f_mag = -0.5 * planetoid_atm(self, position) * coeff * frontal_area * v * v;

    //We take into account it's oppositeness with the negative magnitude.
    double f_azm = vector_azm(velocity);

    //Return the vector.
    return vector_polar(f_mag, f_azm);
}
