#include <stdlib.h>
#include <stdio.h>

#include "rocket.h"
#include "vector.h"

int main(){
    Rocket *r = rocket_init( rocket_alloc() );
    r->position.v[0] = 1.1;
    rocket_dealloc(r);
    printf("%f\n", r->position.v[0]);
    printf("%f\n", r->velocity.v[0]);

    return 0;
}
