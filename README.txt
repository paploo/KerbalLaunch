KerbalLaunch
Jeff Reinecke
jeff@paploo.net

OVERVIEW

Kerbal Launch Trajectory Optimizer.

The eventual goal is to simulate and optimize (using genetic algorithms) the
launch trajectory of rockets in Kerbal Space Program.

At this time it is kept simple: We only handle single-stage rockets.

USE

  make && make run

In the long run, this should output a reasonably optimal flight program for
the rocket launch from Kerbin.

The rocket will eventually be configured from an input file (and the planetoid
parameters as well).

It is also planned to be able to run a flight program and output a CSV file of
its simulated trajectory for further analysis.

Rough benchmarks on 2.8 GHz Intel Core Duo give about 2 million ticks/second on
a single thread.

DESIGN

The program can be divided up into the simulation of a physical system, and the
genetic flight program optimizer.

The flight system is handled by a System struct, which is given (but does not
own) a rocket, planetoid, and program for simulation.  The rocket is
destructively modified during system_run.

The Planetoid encapsulates information about the planet you are launchging from.

The Rocket encapsulates information about the rocket being launched.

The Program contains the logic for setting the throttle and trajectory angle
for the flight profile.  For now this is a lookup-table, but could be substituted
with more complex logic at any time.

The optimizer has yet to be constructed.

TODO

To get full 2D support I need:
[ ]Simulation cutoff:
    [ ] To save on a lot of simulation time, if we are above the atmosphere and
        have reached apoapsis, we can use the rocket equation to assume a delta-v
        and, since we are at an apsis, calculate the resulting alt of hte other
        apsis.  This will allow us to use semimajor-axis of an orbit as a fitness
        function.
    [ ] Escape orbits can be detected if the energy >= 0.
    [+] Assuming we don't simulate burn at apex (which would give us time to fall),
        we can cutoff the simulation when the radial velocity (relative to the
        planetoid) is negative.  To be safer, we can do this when it is negative
        and we are below theedge of the atmosphere, at which point we'd have
        atmospheric decay and a possible--but not idael--orbit.
      * NOTE: http://www.braeunig.us/space/orbmech.htm may help with this.
[ ] Program Addition for a variable cutoff altitude at which we cut-off engines
    and coast to near apoapsis altitude before burning.
[+] Take into account initial velocity from planetoid rotation.
[ ] What is a good fitness function?

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
