#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "program.h"

Program *program_alloc(void) {
    return (Program *)malloc(sizeof(Program));
}

void program_dealloc(Program *self) {
    if(self->altitudes)
        free(self->altitudes);
    if(self->settings)
        free(self->settings);
    free(self);
}

Program *program_init(Program *self, size_t length) {
    self->length = length;
    self->altitudes = (double *)calloc(length, sizeof(double));
    self->settings = (double *)malloc(length*sizeof(double));
    return self;
}

Program *program_init_copy(Program *self, Program *src) {
    program_init(self, src->length);
    size_t length_in_bytes = src->length * sizeof(double);
    memcpy(self->altitudes, src->altitudes, length_in_bytes);
    memcpy(self->settings, src->settings, length_in_bytes);

    return self;
}

double program_lookup(const Program *self, double altitude, int *error) {
    if( altitude < self->altitudes[0] ) {
        *error = 1;
        return 0.0;
    }

    size_t i = 0;
    while(i < self->length-1) {
        if(  altitude < self->altitudes[i+1] )
            break;
        i++;
    }
    *error = 0;
    return self->settings[i];
}

void program_display(const Program *self) {
    printf("<program\n");
    for(size_t i=0; i<self->length; i++) {
        printf("\t[%lu] %6.0f -> %0.3f\n", i, self->altitudes[i], self->settings[i]);
    }
    printf(">\n");
}
