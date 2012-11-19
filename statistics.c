#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "statistics.h"

Statistics *statistics_alloc(void) {
    return (Statistics *)malloc(sizeof(Statistics));
}

void statistics_dealloc(Statistics *self) {
    free(self);
}

Statistics *statistics_init(Statistics *self) {
    memset(self, 0, sizeof(Statistics));
    return self;
}

void statistics_display(const Statistics *self) {
    printf("max_alt        : %f m @ %f sec\n", self->frame.altitude, self->frame.time);
    printf("max_pos        : (%f, %f)\n", VX(self->frame.position), VY(self->frame.position));
    printf("max_alt vel    : (%f, %f)\n", VX(self->frame.velocity), VY(self->frame.velocity));
    printf("max_alt r e,h  : %f, %f\n", self->frame.energy, self->frame.angular_momentum);
    printf("max_alt ra, rp : %f, %f\n", self->frame.apoapsis, self->frame.periapsis);
    printf("distance       : %f m\n", self->distance_travelled);
    printf("work_thrust    : %f J\n", self->work_thrust);
    printf("work_drag      : %f J\n", self->work_drag);
    printf("work_gravity   : %f J\n", self->work_gravity);
    printf("delta_v_thrust : %f m/s\n", self->delta_v_thrust);
    printf("delta_v_drag   : %f m/s\n", self->delta_v_drag);
    printf("delta_v_gravity: %f m/s\n", self->delta_v_gravity);
    printf("delta_v remaing: %f m/s\n", self->frame.rocket_remaining_ideal_delta_v);
}
