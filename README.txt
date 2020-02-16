KerbalLaunch
Jeff Reinecke
jeff@paploo.net


OVERVIEW

Kerbal Launch Trajectory Optimizer.

The eventual goal is to simulate and optimize (using genetic algorithms) the
launch trajectory of rockets in Kerbal Space Program.

At this time it is kept simple: We only handle single-stage rockets.


USE

Original 2012 Script:
  make && make run

Importing into CLion (2020) resulted an the auto creation of a CMake file

In the long run, this should output a reasonably optimal flight program for
the rocket launch from Kerbin.

The rocket will eventually be configured from an input file (and the planetoid
parameters as well).

It is also planned to be able to run a flight program and output a CSV file of
its simulated trajectory for further analysis.

Rough benchmarks on 2.8 GHz Intel Core Duo give in the region of 2 million
ticks/second on asingle thread.


DESIGN

The program can be divided up into the simulation of a physical system, and the
genetic flight program optimizer.


Simulation

The flight system is handled by a System struct, which is given (but does not
own) a rocket, planetoid, and program for simulation.  The rocket is
destructively modified during system_run.

The Planetoid encapsulates information about the planet you are launchging from.

The Rocket encapsulates information about the rocket being launched.

The Program contains the logic for setting the throttle and trajectory angle
for the flight profile.  For now this is a lookup-table, but could be substituted
with more complex logic at any time.

The optimizer has yet to be constructed.


Optimizer

The flight program optimizer is given a seed program and a function pointer for
getting a clean rocket instance.  It then runs the original seed, combined with
N mutations through simulation.  At the end of simulation, the fitness of each
is determined and the one with the highest fitness is taken as the new seed.

Since the system parameters target an altitude for apex, we can calculate the
final semimajor-axis of the orbit assuming all remaining fuel is burned at apex
to raise the orbit periapsis out of the ground.  Note that we cannot use the
remaining fuel (or delta_v) of the rocket directly, because the angular
momentum of the trajectory may be low so the largets attainable orbit may not
even avoid the planetoid surface.

The program optimizer has a method for building N systems with different
programs, and their own rocket instance to mutate.  These systems are then
handed to a method that takes a void pointer to a system and runs it.  It then
returns a void pointer to an allocated result structure that contains the
fitness parameter; this method should conform to pthread start routine function
so that it can be multithreaded, this also means the contents must be thread
safe, hence the desire for fully formed separate system objects with separate
mutatable stats (e.g. rocket instances).


TODO

Genetic Routine:

[ ] Define

LICENSE

Copyright (c) 2012, Jeffrey C. Reinecke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL JEFFREY REINECKE BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
