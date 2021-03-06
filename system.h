#ifndef KERBAL_LAUNCH_SYSTEM_H
#define KERBAL_LAUNCH_SYSTEM_H

#include <stdbool.h>
#include <stdio.h>

#include "rocket.h"
#include "program.h"
#include "planetoid.h"
#include "statistics.h"
#include "frame.h"

#define SYSTEM_TICKS_PER_SECOND 100
#define SYSTEM_LOG_INTERVAL_SECONDS 1
#define SYSTEM_MAX_MISSION_TIME 900.0

typedef enum SystemState {
    SYSTEM_STATE_READY=0,
    SYSTEM_STATE_RUNNING,
    SYSTEM_STATE_SUCCESS,
    SYSTEM_STATE_ERROR=-1
} SystemState;

typedef struct System {
    Statistics stats; //The stats object is a static member of the system; the system itself can be thought of as a stats object.

    Rocket *rocket;
    const Planetoid *planetoid;

    const Program *throttle_program;
    const Program *altitude_angle_program;
    double throttle_cutoff_radius; //If the calculated apoapsis is above this value, cutoff the throttle.

    double delta_t;

    unsigned long ticks;
    SystemState state;
    Frame *frame;

    bool collect_stats; //If true, collect stats.  This slows down simulation rate.

    bool logging;
    FILE *log; //Set this to a file pointer when you want to log to something other than the default (stdout).
} System;

System *system_alloc(void);
void system_dealloc(System *self);
System *system_init(System *self);

void system_run(System *self);
void system_run_one_tick(System *self);

double system_time(const System *self);

Vector system_net_force(const System *self);
void system_set_throttle(System *self);
void system_set_altitude_angle(System *self);

bool system_apses(const System *self, double *periapsis, double *apoapsis);
double system_energy(const System *self);
double system_angular_momentum(const System *self);

void system_update_stats(System *self);
void system_log_header(const System *self);
void system_log_tick(const System *self);

/*
 * Given an angular momentum and an energy, Calculate the periapsis and apopais radius.
 * The return is true if the orbit is closed; the result is placed in the passed pointers.
 * If the orbit is open, false is passed and the values of periapsis abd apoapsis are not defined.
 */
bool orbit_apses(double graviational_parameter, double angluar_momentum, double energy, double *periapsis, double *apoapsis);
double orbit_eccentricity(double graviational_parameter, double angular_momentum, double energy);

#endif
