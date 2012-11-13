#ifndef KERBAL_LAUNCH_STATISTICS_H
#define KERBAL_LAUNCH_STATISTICS_H

typedef struct Statistics {
    double max_radius;
    double max_radius_time;
    double distance_travelled;
    double mission_time;
} Statistics;

Statistics *statistics_alloc(void);
void statistics_dealloc(Statistics *stats);
Statistics *statistics_init(Statistics *stats);

#endif
