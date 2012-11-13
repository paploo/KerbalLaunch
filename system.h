#ifndef KERBAL_LAUNCH_SYSTEM_H
#define KERBAL_LAUNCH_SYSTEM_H

#include "rocket.h"
#include "program.h"
#include "planetoid.h"
#include "statistics.h"

typedef struct System {
    Rocket *rocket;
    Program *throttle_program;
    Planetoid *planetoid;
    double delta_t;

    double time;
    Statistics *stats;
} System;

System *system_alloc(void);
void system_dealloc(System *self);
System *system_init(System *self);

void system_run(System *self);
void system_run_one_tick(System *self);

#endif
