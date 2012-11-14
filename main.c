#include <stdlib.h>
#include <stdio.h>

#include "rocket.h"
#include "vector.h"
#include "program.h"

#define DEGREE 57.29577951308232

int main(){
    //r = 10.0, theta = 210 deg =-150 deg = 3.665191429188092 rad
    Vector v = vector_rect(-8.660254037844387, -5.00);
    printf("(%f, %f)\n", v.v[0], v.v[1]);
    printf("%f @ %f\n", vector_mag(v), vector_azm(v)*DEGREE);

    Program *p = program_init(program_alloc(), 10);
    p->altitudes[7] = 10000;
    p->throttles[9] = 0.9;
    Program *q = program_init_copy(program_alloc(), p);
    program_display(p);
    program_display(q);

    return 0;
}
