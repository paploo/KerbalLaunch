#ifndef KERBAL_LAUNCH_PROGRAM_H
#define KERBAL_LAUNCH_PROGRAM_H

typedef struct Program {
    size_t length;
    double *altitudes;
    double *throttles;
    double *angles;
} Program;

Program *program_alloc(void);
void program_dealloc(Program *self);
Program *program_init(size_t size);

double program_lookup_throttle(double altitude);
double program_lookup_angle(double altitude);

#endif
