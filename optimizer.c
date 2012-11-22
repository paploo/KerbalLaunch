#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>

#include "optimizer.h"

typedef void *(*pthread_func)(void *);

Optimizer *optimizer_alloc(void) {
    return (Optimizer *)malloc(sizeof(Optimizer));
}

void optimizer_dealloc(Optimizer *self) {
    program_dealloc(self->best_throttle_program);
    program_dealloc(self->best_altitude_angle_program);
    free(self);
}

Optimizer *optimizer_init(Optimizer *self) {
    self->rocket_factory_func = NULL;

    self->planetoid = NULL;

    self->seed_throttle_program = NULL;
    self->seed_altitude_angle_program = NULL;
    self->throttle_cutoff_radius = 0.0;

    self->best_throttle_program = NULL;
    self->best_altitude_angle_program = NULL;
    self->best_fitness = -INFINITY;

    srand(time(NULL));
    self->generation = 0;
    self->generations = 1;

    return self;
}

double optimizer_run(Optimizer *self) {
    //Seed programs.
    assert(self->seed_throttle_program != NULL);
    assert(self->seed_altitude_angle_program != NULL);
    self->best_throttle_program = program_init_copy(program_alloc(), self->seed_throttle_program);
    self->best_altitude_angle_program = program_init_copy(program_alloc(), self->seed_altitude_angle_program);

    //Now run.
    while(self->generation < self->generations) {
        printf(".");
        optimizer_run_generation(self);
        self->generation++;
    }

    //Return best fitness value.
    return self->best_fitness;
}

double optimizer_run_generation(Optimizer *self) {
    //Initialize the systems.
    System **systems = optimizer_make_systems(self);

    //Run each system in a thread.
    pthread_t threads[OPTIMIZER_CHILDREN];
    for(unsigned i=0; i<OPTIMIZER_CHILDREN; i++)
        pthread_create(&threads[i], NULL, (pthread_func)optimizer_run_system, systems[i]);

    //Thread join, collect result, and keep if optimal.
    for(unsigned i=0; i<OPTIMIZER_CHILDREN; i++) {
        //Fetch
        OptimizerSystemResult *result = NULL;
        pthread_join(threads[i], (void *)(&result));
        //Keep?
        if(result && (result->fitness > self->best_fitness)) {
            program_dealloc(self->best_throttle_program);
            self->best_throttle_program = program_init_copy(program_alloc(), result->throttle_program);
            program_dealloc(self->best_altitude_angle_program);
            self->best_altitude_angle_program = program_init_copy(program_alloc(), result->altitude_angle_program);
            self->best_fitness = result->fitness;
        }
        //Cleanup
        free(result);
    }

    //Cleanup
    optimizer_destroy_systems(systems);
    return self->best_fitness;
}

OptimizerSystemResult *optimizer_run_system(System *system) {
    //Run system.
    system_run(system);

    //Circularize, and then calculate the excess velocity.
    double excess_delta_v = -INFINITY;

    if( system->state == SYSTEM_STATE_SUCCESS ) {
        double grav_param = system->planetoid->gravitational_parameter;

        double target_radius = system->throttle_cutoff_radius;
        double v_circ = sqrt(grav_param/target_radius);

        double radius = system->stats.frame.radius;

        double initial_horizontal_velocity = fabs( planetoid_horizontal_velocity(system->planetoid, system->stats.frame.position, system->stats.frame.velocity) );
        double rocket_delta_v = rocket_ideal_delta_v(system->rocket);

        /*
         * If we reached target, then we can calculate circularization correctly.
         * If we don't reach target, the calculation neglects air resistance so
         * it makes it appear you can get there with way less fuel.
         * While we could just not let any non-functional orbit score higher than
         * -inf, we still want the best one to win, so we give it a penalized
         * version (it's treated as out of fuel), dropping it down.
         * NOTE: I'm still not sure that this will always work correctly.
         */
        if(radius >= target_radius)
            excess_delta_v = initial_horizontal_velocity + rocket_delta_v - v_circ;
        else
            excess_delta_v = initial_horizontal_velocity - v_circ;

        //printf("%f\t%f\t%f\t%f\t%f\n", excess_delta_v, radius, v_circ, initial_horizontal_velocity, rocket_delta_v);
    }

    //Allocate and fillin result.
    OptimizerSystemResult *result = (OptimizerSystemResult *)malloc(sizeof(OptimizerSystemResult));
    result->throttle_program = system->throttle_program;
    result->altitude_angle_program = system->altitude_angle_program;
    result->fitness = excess_delta_v;

    //Return.
    return result;
}

System **optimizer_make_systems(const Optimizer *self) {
    System **systems = (System **)malloc(sizeof(System *) * OPTIMIZER_CHILDREN);

    for(size_t i=0; i<OPTIMIZER_CHILDREN; i++) {
        systems[i] = system_init(system_alloc());
        systems[i]->planetoid = self->planetoid;
        systems[i]->rocket = optimizer_make_rocket(self);
        systems[i]->throttle_program = optimizer_mutate_throttle_program(self->best_throttle_program);
        systems[i]->altitude_angle_program = optimizer_mutate_altitude_angle_program(self->best_altitude_angle_program);
        systems[i]->throttle_cutoff_radius = self->throttle_cutoff_radius;
    }

    return systems;
}

void optimizer_destroy_systems(System **systems) {
    for(size_t i=0; i<OPTIMIZER_CHILDREN; i++) {
        rocket_dealloc(systems[i]->rocket);
        program_dealloc( (Program *)(systems[i]->throttle_program) ); //Cast from const to non-const for this call.
        program_dealloc( (Program *)(systems[i]->altitude_angle_program) ); //Cast rom const to non-const for this call.
        system_dealloc(systems[i]);
    }
    free(systems);
}

Program *optimizer_mutate_throttle_program(const Program *program) {
    //Copy the seed program.
    Program *mutant_program = program_init_copy(program_alloc(), program);

    //Choose a value to modify.
    size_t i = rand() % program->length;

    //Choose a value for it between 0.0-1.0, with the given number of intervals.
    double throttle = (double)(rand() % (THROTTLE_INTERVALS+1)) / (double)THROTTLE_INTERVALS;

    //Set it
    mutant_program->settings[i] = throttle;

    //Return
    return mutant_program;
}

Program *optimizer_mutate_altitude_angle_program(const Program *program) {
    //Copy the seed program.
    Program *mutant_program = program_init_copy(program_alloc(), program);

    //Choose a value to modify.
    size_t i = rand() % program->length;

    //Choose a value for it.
    double altitude_angle = (M_PI/2.0) * ((double)(rand() % (ALTITUDE_ANGLE_INTERVALS+1)) / (double)ALTITUDE_ANGLE_INTERVALS);

    //Set it
    mutant_program->settings[i] = altitude_angle;

    //Return
    return mutant_program;
}

Rocket *optimizer_make_rocket(const Optimizer *self) {
    return (Rocket *)self->rocket_factory_func(rocket_alloc());
}
