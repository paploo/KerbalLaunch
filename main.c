#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "system.h"

void test();
int simulate_vert();
Rocket *init_small_rocket(Rocket *rocket);
Rocket *init_large_rocket(Rocket *rocket);

int main(){
    return simulate_vert();
}

int simulate_vert() {
    //Build the planetoid
    Planetoid *kerbin = planetoid_init(planetoid_alloc());

    //Build the rocket
    Rocket *rocket = init_small_rocket(rocket_alloc());
    rocket->position.v[1] = kerbin->radius + 72.0; // Small rocket sits at 72.0m on pad.

    //Build the programs
    Program *throttle_program = program_init(program_alloc(), 1);
    throttle_program->altitudes[0] = -600000.0;
    throttle_program->settings[0] = 1.0;

    Program *altitude_angle_program = program_init(program_alloc(), 1);
    altitude_angle_program->altitudes[0] = -600000.0;
    altitude_angle_program->settings[0] = M_PI/2.0;

    //Override settings to test a circular orbit
    /*
    rocket->position.v[0] = 0.0;
    rocket->position.v[1] = 675000.0;
    rocket->velocity.v[0] = sqrt(5232000.0);
    rocket->velocity.v[1] = 0.0;
    throttle_program->settings[0] = 0.0;
    */


    //Build the system
    System *system = system_init(system_alloc());
    system->planetoid = kerbin;
    system->rocket = rocket;
    system->throttle_program = throttle_program;
    system->altitude_angle_program = altitude_angle_program;
    system->logging = true;

    //Open the log
    if(system->logging) {
        system->log = fopen("_rocket.csv", "w+");
    }

    //Run
    system_run(system);
    /*
    system_log_header(system);
    system_run_one_tick(system);
    */

    //Close the log
    if(system->logging) {
        fclose(system->log);
        system->log = stdout;
    }

    //Output stats
    printf("state     : %d\n", system->state);
    printf("ticks     : %lu\n", system->ticks);

    Statistics *stats = &system->stats;
    printf("time      : %f s\n", stats->mission_time);
    printf("max_alt   : %f m @ %f sec\n", stats->max_altitude, stats->max_radius_time);
    printf("max_pos   : (%f, %f)\n", VX(stats->max_radius_position), VY(stats->max_radius_position));
    printf("max_vel   : (%f, %f)\n", VX(stats->max_radius_velocity), VY(stats->max_radius_velocity));
    printf("max r e,h : %f, %f\n", stats->max_radius_energy, stats->max_radius_angular_momentum);
    printf("max ra, rp: %f, %f\n", stats->max_radius_apoapsis, stats->max_radius_periapsis);
    printf("distance  : %f m\n", stats->distance_travelled);
    printf("delta_v_thrust   : %f m/s\n", stats->delta_v_thrust);
    printf("delta_v_drag     : %f m/s\n", stats->delta_v_drag);
    printf("delta_v_gravity  : %f m/s\n", stats->delta_v_gravity);

    //Cleanup
    system_dealloc(system);
    program_dealloc(throttle_program);
    program_dealloc(altitude_angle_program);
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

    rocket->max_drag = 0.2;

    return rocket;
}

Rocket *init_large_rocket(Rocket *rocket) {
    rocket_init(rocket);

    rocket->mass = 15.70;
    rocket->empty_mass = 15.70-12.0;

    rocket->max_thrust = 215.0;
    rocket->isp_vac = 370.0;
    rocket->isp_atm = 320.0;

    rocket->max_drag = 0.2;

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
