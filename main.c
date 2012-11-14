#include <stdlib.h>
#include <stdio.h>

#include "rocket.h"
#include "vector.h"

#define DEGREE 57.29577951308232

int main(){
    //r = 10.0, theta = 210 deg =-150 deg = 3.665191429188092 rad
    Vector v = vector_rect(-8.660254037844387, -5.00);
    printf("(%f, %f)\n", v.v[0], v.v[1]);
    printf("%f @ %f\n", vector_mag(v), vector_azm(v)*DEGREE);

    return 0;
}
