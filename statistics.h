#ifndef KERBAL_LAUNCH_STATISTICS_H
#define KERBAL_LAUNCH_STATISTICS_H

typedef struct Statistics {
    double mission_time;
    double max_radius;
    double max_altitude;
    double max_radius_time;
    double distance_travelled;

    double delta_v_thrust;
    double delta_v_drag;
    double delta_v_gravity;
} Statistics;

Statistics *statistics_alloc(void);
void statistics_dealloc(Statistics *stats);
Statistics *statistics_init(Statistics *stats);

#endif
