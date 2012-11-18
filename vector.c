#include <stdio.h>
#include <math.h>

#include "vector.h"

Vector vector() {
    Vector v = {{0,0}};
    return v;
}

Vector vector_rect(double x, double y) {
    Vector v;
    v.v[0] = x;
    v.v[1] = y;
    return v;
}

Vector vector_polar(double mag, double azm) {
    Vector v;
    v.v[0] = mag * cos(azm);
    v.v[1] = mag * sin(azm);
    return v;
}

double vector_mag(Vector v) {
    return sqrt(vector_inner(v,v));
}

double vector_azm(Vector v) {
    return atan2(v.v[1], v.v[0]);
}

Vector vector_rotate(Vector v, double theta) {
    double xprime = VX(v)*cos(theta) - VY(v)*sin(theta);
    double yprime = VX(v)*sin(theta) + VY(v)*cos(theta);
    return vector_rect(xprime, yprime);
}

double vector_inner(Vector v, Vector u) {
    double sum = 0.0;
    for(size_t i=0; i<VECTOR_DIMS; i++)
        sum += (u.v[i]*v.v[i]);
    return sum;
}

double vector_cross(Vector v, Vector u) {
    return v.v[0]*u.v[1] -  v.v[1]*u.v[0];
}

Vector vector_add(Vector v, Vector u) {
    Vector w;
    for(size_t i=0; i<VECTOR_DIMS; i++)
        w.v[i] = v.v[i] + u.v[i];
    return w;
}

Vector vector_sub(Vector v, Vector u) {
    Vector w;
    for(size_t i=0; i<VECTOR_DIMS; i++)
        w.v[i] = v.v[i] - u.v[i];
    return w;
}
