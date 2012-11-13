#include <stdlib.h>

#include "system.h"

System *system_alloc(void) {
    return (System *)malloc(sizeof(System));
}

void system_dealloc(System *self) {
    statistics_dealloc(self->stats);
    return free(self);
}

System *system_init(System *self) {
    self->rocket = NULL;
    self->throttle_program = NULL;
    self->planetoid = NULL;

    self->delta_t = 1.0/100.0;

    self->time = 0.0;
    self->stats = statistics_init(statistics_alloc());

    return self;
}

void system_run(System *self) {
}

void system_run_one_tick(System *self) {
}

