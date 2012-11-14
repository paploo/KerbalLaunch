#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "program.h"

Program *program_alloc(void) {
    return (Program *)malloc(sizeof(Program));
}

void program_dealloc(Program *self) {
    free(self->altitudes);
    free(self->throttles);
    free(self->bearings);
    free(self);
}

Program *program_init(Program *self, size_t length) {
    self->length = length;
    self->altitudes = (double *)calloc(length, sizeof(double));
    self->throttles = (double *)malloc(length*sizeof(double));
    self->bearings = (double *)calloc(length, sizeof(double));
    return self;
}

Program *program_init_copy(Program *self, Program *src) {
    program_init(self, src->length);
    size_t length_in_bytes = src->length * sizeof(double);
    memcpy(self->altitudes, src->altitudes, length_in_bytes);
    memcpy(self->throttles, src->throttles, length_in_bytes);
    memcpy(self->bearings, src->bearings, length_in_bytes);

    return self;
}

void program_display(Program *self) {
    printf("<program\n");
    for(size_t i=0; i<self->length; i++) {
        printf("\t[%lu] %6.0f -> (%0.3f, %0.3f)\n", i, self->altitudes[i], self->throttles[i], self->bearings[i]);
    }
    printf(">\n");
}
