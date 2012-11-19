#ifndef KERBAL_LAUNCH_STATISTICS_H
#define KERBAL_LAUNCH_STATISTICS_H

#include "vector.h"
#include "frame.h"

/* Statistics is a frame with aggregate values */
typedef struct Statistics {
    Frame frame;

    double distance_travelled;

    double delta_v_thrust;
    double delta_v_drag;
    double delta_v_gravity;

    double work_thrust;
    double work_drag;
    double work_gravity;
} Statistics;

Statistics *statistics_alloc(void);
void statistics_dealloc(Statistics *stats);
Statistics *statistics_init(Statistics *stats);

void statistics_display(const Statistics *stats);

#endif
