#ifndef KERBAL_LAUNCH_VECTOR_H
#define KERBAL_LAUNCH_VECTOR_H

#define VECTOR_DIMS 2

typedef struct Vector {
    double v[2];
} Vector;

inline Vector vector();
inline Vector vector_rect(double x, double y);
Vector vector_polar(double mag, double azm);

double vector_mag(Vector v);
double vector_azm(Vector v);

double vector_inner(Vector v, Vector u);

Vector vector_add(Vector v, Vector u);
Vector vector_sub(Vector v, Vector u);

#endif
