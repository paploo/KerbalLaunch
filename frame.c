#include <stdio.h>

#include "frame.h"

void frame_display(const Frame *frame) {
    printf("----------\n");
    printf("ticks: %lu, t: %f\n", frame->ticks, frame->t);
    printf("m : %f, r : (%f, %f), dv: (%f, %f)\n", frame->mass, VX(frame->position), VY(frame->position), VX(frame->velocity), VY(frame->velocity));
    printf("dm: %f, dr: (%f, %f),  v: (%f, %f)\n", frame->delta_mass, VX(frame->delta_position), VY(frame->delta_position), VX(frame->delta_velocity), VY(frame->delta_velocity));
    printf("----------\n");
}
