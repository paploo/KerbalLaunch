#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "system.h"

#define DEGREE 57.29577951308232

void test();
int simulate();
Rocket *init_small_rocket(Rocket *rocket);
Rocket *init_large_rocket(Rocket *rocket);

int main(){
    return simulate();
}

int simulate() {
    //Build the planetoid
    Planetoid *kerbin = planetoid_init(planetoid_alloc());

    //Build the rocket
    Rocket *rocket = init_small_rocket(rocket_alloc());
    rocket->position.v[1] = kerbin->radius + 10.0;

    //Build the programs
    Program *throttle_program = program_init(program_alloc(), 1);
    throttle_program->altitudes[0] = -600000.0;
    throttle_program->settings[0] = 1.0;

    Program *bearing_program = program_init(program_alloc(), 1);
    bearing_program->altitudes[0] = -600000.0;
    bearing_program->settings[0] = M_PI/2.0;


    //Build the system
    System *system = system_init(system_alloc());
    system->planetoid = kerbin;
    system->rocket = rocket;
    system->throttle_program = throttle_program;
    system->bearing_program = bearing_program;
    system->logging = 0;

    //Run
    system_run(system);
    //system_log_header(system);
    //system_run_one_tick(system);

    //Output stats
    printf("state     : %d\n", system->state);
    printf("ticks     : %lu\n", system->ticks);

    Statistics *stats = &system->stats;
    printf("time      : %f s\n", stats->mission_time);
    printf("max_radius: %f m @ %f sec\n", stats->max_radius, stats->max_radius_time);
    printf("distance  : %f m\n", stats->distance_travelled);
    printf("delta_v   : %f m/s\n", stats->delta_v);

    //Cleanup
    system_dealloc(system);
    program_dealloc(throttle_program);
    program_dealloc(bearing_program);
    rocket_dealloc(rocket);
    planetoid_dealloc(kerbin);

    return 0;
}

Rocket *init_small_rocket(Rocket *rocket) {
    rocket_init(rocket);

    rocket->mass = 4.45;
    rocket->empty_mass = 4.45-2.0;

    rocket->max_thrust = 215.0;
    rocket->isp_vac = 370.0;
    rocket->isp_atm = 320.0;

    rocket->frontal_area = 3.14;
    rocket->drag_coefficient = 0.001;

    return rocket;
}

Rocket *init_large_rocket(Rocket *rocket) {
    rocket_init(rocket);

    rocket->mass = 15.70;
    rocket->empty_mass = 15.70-12.0;

    rocket->max_thrust = 215.0;
    rocket->isp_vac = 370.0;
    rocket->isp_atm = 320.0;

    rocket->frontal_area = 3.14;
    rocket->drag_coefficient = 0.001;

    return rocket;
}


void test() {
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
    p->settings[0] = 100.0;
    p->altitudes[1] = 10000.0;
    p->settings[1] = 50.0;
    p->altitudes[2] = 70000.0;
    p->settings[2] = 10.0;

    Program *q = program_init_copy(program_alloc(), p);
    program_display(p);
    program_display(q);

    double alt = 52;
    int err;
    double throttle = program_lookup(p, alt, &err);
    printf("%f -> %f, %d\n", alt, throttle, err);

    program_dealloc(p);
    program_dealloc(q);
}
