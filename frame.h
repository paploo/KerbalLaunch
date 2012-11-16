#ifndef KERBAL_LAUNCH_FRAME_H
#define KERBAL_LAUNCH_FRAME_H

#include "vector.h"

typedef struct Frame {
    unsigned long ticks;

    double t;
    double mass;
    Vector position;
    Vector velocity;

    double delta_t;
    double delta_mass;
    Vector delta_position;
    Vector delta_velocity;

    double radius;
    double altitude;
    double azimuth;

    Vector force;
    Vector force_thrust;
    Vector force_gravity;
    Vector force_drag;

    double throttle;
    double altitude_angle;
} Frame;

#endif
