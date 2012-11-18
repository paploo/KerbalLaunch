#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "system.h"

int simulate_vert();
Rocket *init_small_rocket(Rocket *rocket);
Rocket *init_large_rocket(Rocket *rocket);

int main(){
    clock_t start = clock();
    int result = simulate_vert();
    clock_t stop = clock();
    clock_t delta_t = stop-start;
    printf("TIME: %f ms\n", ((double)delta_t)/CLOCKS_PER_SEC);
    return result;
}

int simulate_vert() {
    //Build the planetoid
    Planetoid *kerbin = planetoid_init(planetoid_alloc());

    //Build the rocket
    Rocket *rocket = init_small_rocket(rocket_alloc());
    rocket->position.v[1] = kerbin->radius + 72.0; // Small rocket sits at 72.0m on pad.
    rocket->velocity.v[0] = kerbin->radius * 0.0002908882086657216; // Surface rotational velocity.

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
    system->collect_stats = true;

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
    printf("state          : %d\n", system->state);
    printf("ticks          : %lu\n", system->ticks);
    if(system->collect_stats)
        statistics_display(&system->stats);

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
