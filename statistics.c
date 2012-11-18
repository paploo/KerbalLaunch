#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "statistics.h"

Statistics *statistics_alloc(void) {
    return (Statistics *)malloc(sizeof(Statistics));
}

void statistics_dealloc(Statistics *stats) {
    free(stats);
}

Statistics *statistics_init(Statistics *stats) {
    memset(stats, 0, sizeof(Statistics));
    return stats;
}

void statistics_display(const Statistics *stats) {
    printf("time           : %f s\n", stats->mission_time);
    printf("max_alt        : %f m @ %f sec\n", stats->max_altitude, stats->max_radius_time);
    printf("max_pos        : (%f, %f)\n", VX(stats->max_radius_position), VY(stats->max_radius_position));
    printf("max_alt vel    : (%f, %f)\n", VX(stats->max_radius_velocity), VY(stats->max_radius_velocity));
    printf("max_alt r e,h  : %f, %f\n", stats->max_radius_energy, stats->max_radius_angular_momentum);
    printf("max_alt ra, rp : %f, %f\n", stats->max_radius_apoapsis, stats->max_radius_periapsis);
    printf("distance       : %f m\n", stats->distance_travelled);
    printf("work_thrust    : %f J\n", stats->work_thrust);
    printf("work_drag      : %f J\n", stats->work_drag);
    printf("work_gravity   : %f J\n", stats->work_gravity);
    printf("delta_v_thrust : %f m/s\n", stats->delta_v_thrust);
    printf("delta_v_drag   : %f m/s\n", stats->delta_v_drag);
    printf("delta_v_gravity: %f m/s\n", stats->delta_v_gravity);
}
