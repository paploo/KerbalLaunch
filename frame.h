#ifndef KERBAL_LAUNCH_FRAME_H
#define KERBAL_LAUNCH_FRAME_H

#include "vector.h"
#include "stdbool.h"

typedef struct Frame {
    unsigned long ticks;

    double time;
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

    double energy;
    double angular_momentum;
    bool closed_orbit;
    double apoapsis;
    double periapsis;

    double rocket_remaining_fuel_mass;
    double rocket_remaining_ideal_delta_v;

    Vector force;
    Vector force_thrust;
    Vector force_gravity;
    Vector force_drag;

    double throttle;
    double altitude_angle;
} Frame;

Frame *frame_init(Frame *self);

void frame_display(const Frame *frame);

#endif
