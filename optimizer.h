#ifndef KERBAL_LAUNCH_OPTIMIZER_H
#define KERBAL_LAUNCH_OPTIMIZER_H

#include "program.h"
#include "planetoid.h"
#include "rocket.h"
#include "system.h"

#define OPTIMIZER_CHILDREN 16
#define THROTTLE_INTERVALS 12 //N intervals means throttle settings will be in [0.0,1.0] with step 1/N.
#define ALTITUDE_ANGLE_INTERVALS 18 //N intervals means throttle settings will be in [0.0,2*PI] with step 2*PI/N.

typedef void *(*InitFunc)(void *);

typedef struct OptimizerSystemResult {
    double fitness;
    const Program *throttle_program;
    const Program *altitude_angle_program;
} OptimizerSystemResult;

typedef struct Optimizer {
    // The function to call to get a fresh rocket instance for simulation.
    InitFunc rocket_factory_func;

    const Planetoid *planetoid;

    const Program *seed_throttle_program;
    const Program *seed_altitude_angle_program;
    double throttle_cutoff_radius;

    Program *best_throttle_program;
    Program *best_altitude_angle_program;
    double best_fitness;

    unsigned generation;
    unsigned generations;
} Optimizer;

Optimizer *optimizer_alloc(void);
void optimizer_dealloc(Optimizer *self);
Optimizer *optimizer_init(Optimizer *self);

double optimizer_run(Optimizer *self);

double optimizer_run_generation(Optimizer *self);
OptimizerSystemResult *optimizer_run_system(System *system); //Must be p_thread thread_function compliant sig.

System **optimizer_make_systems(const Optimizer *self);
void optimizer_destroy_systems(System **systems);
Rocket *optimizer_make_rocket(const Optimizer *self);
Program *optimizer_mutate_throttle_program(const Program *program);
Program *optimizer_mutate_altitude_angle_program(const Program *program);
Program *optimizer_make_copy_program(const Program *program);

#endif
