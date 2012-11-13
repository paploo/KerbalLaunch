#include <stdlib.h>

#include "statistics.h"

Statistics *statistics_alloc(void) {
    return (Statistics *)malloc(sizeof(Statistics));
}

void statistics_dealloc(Statistics *stats) {
    free(stats);
}

Statistics *statistics_init(Statistics *stats) {
    stats->max_radius = 0.0;
    stats->max_radius_time = 0.0;
    stats->distance_travelled = 0.0;
    stats->mission_time = 0.0;

    return stats;
}
