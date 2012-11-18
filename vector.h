#ifndef KERBAL_LAUNCH_VECTOR_H
#define KERBAL_LAUNCH_VECTOR_H

#include <math.h>

#define DEGREE 57.29577951308232

#define VECTOR_DIMS 2

#define VX(vec) (vec.v[0])
#define VY(vec) (vec.v[1])

#ifdef KERBAL_LAUNCH_FLOAT_TRIG
#define kerbal_atan2(y,x) (atan2f((y),(x)))
#define kerbal_sin(theta) (sinf(theta))
#define kerbal_cos(theta) (cosf(theta))
#else
#define kerbal_atan2(y,x) (atan2((y),(x)))
#define kerbal_sin(theta) (sin(theta))
#define kerbal_cos(theta) (cos(theta))
#endif

/*
One might think these are faster because it acts inline, but this one change
increases a rocket launch simulation time by about 10% in my benchmarks!
#define vector_mag(vec) (sqrt( vec.v[0]*vec.v[0] + vec.v[1]*vec.v[1]))
#define vector_azm(vec) (atan2( vec.v[1], vec.v[0]))
*/

typedef struct Vector {
    double v[2];
} Vector;

inline Vector vector();
inline Vector vector_rect(double x, double y);
Vector vector_polar(double mag, double azm);

double vector_mag(Vector v);
double vector_azm(Vector v);

Vector vector_rotate(Vector v, double theta);

double vector_inner(Vector v, Vector u);
/* mag(v x u) = z-component of v x u. */
double vector_cross(Vector v, Vector u);

Vector vector_add(Vector v, Vector u);
/* Calculate v-u */
Vector vector_sub(Vector v, Vector u);

#endif
