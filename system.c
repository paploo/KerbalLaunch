#include <stdlib.h>

#include "system.h"

System *system_alloc(void) {
    return (System *)malloc(sizeof(System));
}

void system_dealloc(System *self) {
    free(self);
}

System *system_init(System *self) {
    self->rocket = NULL;
    self->throttle_program = NULL;
    self->planetoid = NULL;

    self->delta_t = 1.0/100.0;

    self->time = 0.0;
    statistics_init(&self->stats);

    return self;
}

void system_run(System *self) {
}

void system_run_one_tick(System *self) {
}

