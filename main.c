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
    printf("(%f, %f)\n", VX(v), VY(v));
    printf("%f @ %f\n", vector_mag(v), vector_azm(v)*DEGREE);

    printf("\n");

    Rocket *rocket = rocket_init(rocket_alloc());
    rocket->position = vector_rect(3.0,4.0);
    printf("(%f, %f)\n", VX(rocket->position), VY(rocket->position));

    printf("\n");

    Program *p = program_init(program_alloc(), 3);
    p->altitudes[0] = 0.0;
    p->throttles[0] = 100.0;
    p->altitudes[1] = 10000.0;
    p->throttles[1] = 50.0;
    p->altitudes[2] = 70000.0;
    p->throttles[2] = 10.0;

    Program *q = program_init_copy(program_alloc(), p);
    program_display(p);
    program_display(q);

    double alt = 52;
    int err;
    double throttle = program_lookup(p, p->throttles, alt, &err);
    printf("%f -> %f, %d\n", alt, throttle, err);

    return 0;
}
