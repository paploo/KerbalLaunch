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
    self->max_atmospheric_altitude = self->atmospheric_attenuation * log(1e6);

    return self;
}

double planetoid_atm(const Planetoid *self, Vector position) {
    double r = planetoid_position_radius(self, position);
    double a = r - self->radius;
    if( a >= self->max_atmospheric_altitude )
        return 0.0;
    else if( a >= 0.0 )
        return exp(-a / self->atmospheric_attenuation);
    else
        return 1.0;
}

double planetoid_rho(const Planetoid *self, Vector position) {
    double atm = planetoid_atm(self, position);
    return atm * 1.2230948554874 * 0.008; /* This came from the wiki */
}

Vector planetoid_relative_position(const Planetoid *self, Vector position) {
    return vector_sub(position, self->position);
}

double planetoid_position_radius(const Planetoid *self, Vector position) {
    return vector_mag( planetoid_relative_position(self, position) );
}

double planetoid_position_altitude(const Planetoid *self, Vector position) {
    return planetoid_position_radius(self, position) - self->radius;
}

double planetoid_position_azm(const Planetoid *self, Vector position) {
    return vector_azm( planetoid_relative_position(self, position) );
}

Vector planetoid_gravitational_force(const Planetoid *self, double mass, Vector position) {
    //Compute the relative position vector, r
    Vector r_vec = planetoid_relative_position(self, position);
    double r = vector_mag(r_vec);

    //Compute the gravitational force magnitude. Note that it is negative because it goes opposite the position.
    double f_mag = -(mass * self->gravitational_parameter)/(r*r);

    //Compute the gravitational force angle. We take into account it's oppositeness with the negative magnitude.
    double f_azm = vector_azm(r_vec);

    //Return the vector.
    return vector_polar(f_mag, f_azm);
}

/*
 * Drag is not normal.
 *
 * Wiki Nuggets: http://kspwiki.nexisonline.net/wiki/Atmosphere
 * 0.5 * rho * m * d * A * v^2
 * A = 1.0 for all ships.
 * d is max drag for a part, so usually 0.2.
 * rho = p * 1.2230948554874 * 0.008
 * No atmosphere over one-millionth of S.L. pressure.
 *
 * Forum Nuggets : http://forum.kerbalspaceprogram.com/showthread.php/5235-Atmospheric-drag/
 * FD = 0.5*density*velocity^2*maximum_drag*mass
 * density ~ 0.01 Exp(-height/5000 m) km/meter^3
 */
Vector planetoid_atmospheric_drag(const Planetoid *self, Vector position, Vector velocity, double drag, double mass) {
    double v = vector_mag(velocity);

    double rho = planetoid_rho(self, position);

    double f_mag = -0.5 * rho * mass * drag * v * v;
    double f_azm = vector_azm(velocity);
    return vector_polar(f_mag, f_azm);
}

Vector planetoid_irl_atmospheric_drag(const Planetoid *self, Vector position, Vector velocity, double frontal_area, double coeff) {
    double v = vector_mag(velocity);

    //Calculate the magnitude.  Note that it is negative because it goes opposite the velocity.
    double f_mag = -0.5 * planetoid_rho(self, position) * coeff * frontal_area * v * v;

    //We take into account it's oppositeness with the negative magnitude.
    double f_azm = vector_azm(velocity);

    //Return the vector.
    return vector_polar(f_mag, f_azm);
}
