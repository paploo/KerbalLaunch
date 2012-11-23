#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "system.h"
#include "optimizer.h"

#define OPTIMIZATION_SYSTEM_RUNS (16384)

#define TWELFTH 0.16666666666666666
#define FIFTEENTH 0.06666666666666667

int optimize(void);
void simulate_optimized_system(Optimizer *optimizer);

int simulate_vertical(void);

Rocket *init_small_rocket(Rocket *rocket);
Rocket *init_large_rocket(Rocket *rocket);

Program *init_throttle_seed(Program *program);
Program *init_altitude_angle_seed(Program *program);

//Given a system who terminated at apex, calculate the best periapsis/apoapsis.
bool biggest_orbit(const System *system, double *periapsis, double *apoapsis);

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
    Program *seed_throttle_program = program_init(program_alloc(), 9);
    init_throttle_seed(seed_throttle_program);

    Program *seed_altitude_angle_program = program_init(program_alloc(), 9);
    init_altitude_angle_seed(seed_altitude_angle_program);

    double throttle_cutoff_radius = kerbin_radius + 80000.0;

    //Build the optimizer
    Optimizer *optimizer = optimizer_init(optimizer_alloc());
    optimizer->rocket_factory_func = (InitFunc)init_large_rocket;
    optimizer->planetoid = kerbin;
    optimizer->seed_throttle_program = seed_throttle_program;
    optimizer->seed_altitude_angle_program = seed_altitude_angle_program;
    optimizer->throttle_cutoff_radius = throttle_cutoff_radius;
    //optimizer->generations = (64*16)/OPTIMIZER_CHILDREN;
    optimizer->generations = OPTIMIZATION_SYSTEM_RUNS/OPTIMIZER_CHILDREN;

    //Run
    optimizer_run(optimizer);

    //Show Best Result
    printf("Generations x Children: %d x %d = %d\n", optimizer->generation, OPTIMIZER_CHILDREN, OPTIMIZER_CHILDREN*optimizer->generations);
    printf("Fitness: %f\n", optimizer->best_fitness);
    printf("Throttle Program:\n");
    program_display(optimizer->best_throttle_program);
    printf("Altitude Angle Program:\n");
    program_display_converted(optimizer->best_altitude_angle_program, 180.0/M_PI);

    //Simulate best program to gather statistics.
    simulate_optimized_system(optimizer);

    //Cleanup
    optimizer_dealloc(optimizer);
    planetoid_dealloc(kerbin);
    program_dealloc(seed_throttle_program);
    program_dealloc(seed_altitude_angle_program);

    return 0;
}

void simulate_optimized_system(Optimizer *optimizer) {
    // Create the system.
    System *system = system_init(system_alloc());
    system->planetoid = optimizer->planetoid;
    system->rocket = optimizer_make_rocket(optimizer);
    system->throttle_program = optimizer->best_throttle_program;
    system->altitude_angle_program = optimizer->best_altitude_angle_program;
    system->throttle_cutoff_radius = optimizer->throttle_cutoff_radius;
    system->logging = true;
    system->collect_stats = true;

    //Simulate
    system->log = fopen("_optimized_rocket.csv", "w+");

    system_run(system);

    fclose(system->log);
    system->log = stdout;

    //Calculate biggest possible orbit
    if( system->state == SYSTEM_STATE_SUCCESS ) {
        double periapsis = 0.0;
        double apoapsis = 0.0;
        biggest_orbit(system, &periapsis, &apoapsis);

        printf("Largest Orbit:\n");
        printf("periapsis radus: %f m\n", periapsis);
        printf("apoapsis radius: %f m\n", apoapsis);
    }


    //Display Output
    printf("Apex Frame:\n");
    statistics_display(&system->stats);

    //Cleanup
    free(system->rocket);
    free(system);
}

bool biggest_orbit(const System *system, double *periapsis, double *apoapsis) {
    double grav_param = system->planetoid->gravitational_parameter;

    double radius = system->stats.frame.radius;
    double initial_horizontal_velocity = planetoid_horizontal_velocity(system->planetoid, system->stats.frame.position, system->stats.frame.velocity);

    double rocket_delta_v = rocket_ideal_delta_v(system->rocket);

    double velocity = fabs(initial_horizontal_velocity) + rocket_delta_v; //fabs because direction doesn't matter.

    double angular_momentum = radius * velocity; //Only works this nicely at apsis.
    double energy = 0.5*velocity*velocity - grav_param/radius;

    return orbit_apses(grav_param, angular_momentum, energy, periapsis, apoapsis);
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

Program *init_throttle_seed(Program *program) {
    const size_t dim = 9;
    assert(program->length == dim);
    double altitudes[] = {-600000.0, 1000.0, 2000.0, 5000.0, 12000.0, 23000.0, 35000.0, 45000.0, 60000.0};
    //double settings[] = {1.0, 0.9, 0.7, 0.5, 0.6, 0.8, 0.8, 0.9, 1.0};
    double settings[] = {15, 15, 15, 15, 15, 8, 12, 4, 8};
    for(size_t i=0; i<dim; i++) {
        program->altitudes[i] = altitudes[i];
        program->settings[i] = settings[i] * FIFTEENTH;
    }
    return program;
}

Program *init_altitude_angle_seed(Program *program) {
    const size_t dim = 9;
    assert(program->length == dim);
    double altitudes[] = {-600000.0, 1000.0, 2000.0, 5000.0, 12000.0, 23000.0, 35000.0, 45000.0, 60000.0};
    //double settings[] = {90*DEGREE, 85*DEGREE, 85*DEGREE, 90*DEGREE, 45*DEGREE, 30*DEGREE, 10*DEGREE, 5*DEGREE, 0*DEGREE};
    double settings_degree[] ={90.0, 90.0, 90.0, 85.0, 50.0, 20.0, 10.0, 5.0, 0.0};
    for(size_t i=0; i<dim; i++) {
        program->altitudes[i] = altitudes[i];
        program->settings[i] = settings_degree[i] * DEGREE;
    }
    return program;
}
