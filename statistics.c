#include <stdlib.h>
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
