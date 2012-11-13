#include <stdlib.h>
#include <stdio.h>

#include "rocket.h"

int main(){
    Rocket *r = rocket_init( rocket_alloc() );
    r->position[0] = 1.1;
    rocket_dealloc(r);
    printf("%f\n", r->position[0]);
    printf("%f\n", r->velocity[0]);
    return 0;
}
