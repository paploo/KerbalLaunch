#ifndef KERBAL_LAUNCH_VECTOR_H
#define KERBAL_LAUNCH_VECTOR_H

#define VECTOR_DIMS 2

typedef struct Vector {
    double v[2];
} Vector;

double vector_mag(Vector v);
double vector_inner(Vector v, Vector u);
Vector vector_add(Vector v, Vector u);
Vector vector_sub(Vector v, Vector u);

#endif
