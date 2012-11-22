#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "system.h"
#include "optimizer.h"

int optimize(void);
int simulate_vertical(void);

Rocket *init_small_rocket(Rocket *rocket);
Rocket *init_large_rocket(Rocket *rocket);

double kerbin_radius;

int main(void){
    clock_t start = clock();
    int result = optimize();
    clock_t stop = clock();
    clock_t delta_t = stop-start;
    printf("TIME: %f ms\n", ((double)delta_t)/CLOCKS_PER_SEC);
    return result;
}

int optimize(void) {
    //Build the planetoid
    Planetoid *kerbin = planetoid_init(planetoid_alloc());
    kerbin_radius = kerbin->radius;

    //Builde the seed programs.
    Program *seed_throttle_program = program_init(program_alloc(), 1);
    seed_throttle_program->altitudes[0] = -kerbin_radius;
    seed_throttle_program->settings[0] = 1.0;

    Program *seed_altitude_angle_program = program_init(program_alloc(), 1);
    seed_altitude_angle_program->altitudes[0] = -kerbin_radius;
    seed_altitude_angle_program->settings[0] = M_PI*2.0;

    double throttle_cutoff_radius = kerbin_radius + 80000.0;

    //Build the optimizer
    Optimizer *optimizer = optimizer_init(optimizer_alloc());
    optimizer->rocket_factory_func = (InitFunc)init_large_rocket;
    optimizer->planetoid = kerbin;
    optimizer->seed_throttle_program = seed_throttle_program;
    optimizer->seed_altitude_angle_program = seed_altitude_angle_program;
    optimizer->throttle_cutoff_radius = throttle_cutoff_radius;

    //Run
    optimizer_run(optimizer);

    //Show Best Result
    printf("Generation: %d", optimizer->generation);
    printf("Fitness: %f\n", optimizer->best_fitness);
    printf("Throttle Program:\n");
    program_display(optimizer->best_throttle_program);
    printf("Altitude Angle Program:\n");
    program_display(optimizer->best_altitude_angle_program);

    //Simulate best program to gather statistics.
    //TODO: Simulate best program to gather stats.

    //Cleanup
    optimizer_dealloc(optimizer);
    planetoid_dealloc(kerbin);
    program_dealloc(seed_throttle_program);
    program_dealloc(seed_altitude_angle_program);

    return 0;
}

int simulate_vertical(void) {
    //Build the planetoid
    Planetoid *kerbin = planetoid_init(planetoid_alloc());
    kerbin_radius = kerbin->radius;

    //Build the rocket
    Rocket *rocket = init_large_rocket(rocket_alloc());

    //Build the programs
    Program *throttle_program = program_init(program_alloc(), 1);
    throttle_program->altitudes[0] = -600000.0;
    throttle_program->settings[0] = 1.0;

    Program *altitude_angle_program = program_init(program_alloc(), 1);
    altitude_angle_program->altitudes[0] = -600000.0;
    altitude_angle_program->settings[0] = M_PI/2.0;

    double throttle_cutoff_radius = kerbin->radius + 80000;

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
    system->throttle_cutoff_radius = throttle_cutoff_radius;
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
    printf("sys state      : %d\n", system->state);
    printf("sys ticks      : %lu s\n", system->ticks);
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

    rocket->position.v[1] = kerbin_radius + 72.0; // Small rocket sits at 72.0m on pad.
    rocket->velocity.v[0] = kerbin_radius * 0.0002908882086657216; // Surface rotational velocity.

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

    rocket->position.v[1] = kerbin_radius + 72.0; // Small rocket sits at 72.0m on pad.
    rocket->velocity.v[0] = kerbin_radius * 0.0002908882086657216; // Surface rotational velocity.

    return rocket;
}
