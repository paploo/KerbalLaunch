#ifndef KERBAL_LAUNCH_PROGRAM_H
#define KERBAL_LAUNCH_PROGRAM_H

typedef struct Program {
    size_t length;
    double *altitudes;
    double *throttles;
    double *bearings;
} Program;

Program *program_alloc(void);
void program_dealloc(Program *self);
Program *program_init(Program *self, size_t length);
Program *program_init_copy(Program *self, Program *src);

double program_lookup(const Program *self, const double *table, double input, int *error);

void program_display(const Program *self);

#endif
