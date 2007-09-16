/**  
  *  Random number wrapper
  *
 **/
#define _TCANETPP_RANDOM_C_

#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#include "random.h"


static int _seeded = 0;


static void
Seed()
{
    long    x, y;
    int     s;
    time_t  t;
    ldiv_t  ld;

    x = (long) time(&t);
    y = ( x / 5555 );

    ld = ldiv(x, y);
    s  = (int) ld.rem;

    srand(s);

    return;
}

static uint32_t
RandomValue ( double range ) 
{
    long s = abs((long)((range * rand()) / (RAND_MAX + 1.0)));
    return (uint32_t) s;
}



void
tcanet_seed()
{
    Seed();
    _seeded = 1;
}


uint32_t
tcanet_randomValue ( double range )
{
    if ( ! _seeded )
	tcanet_seed();
    return RandomValue(range);
}



//  _TCANETPP_RANDOM_C_
