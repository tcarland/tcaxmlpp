/**  
  *  Random number wrapper
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
  *
  * This file is part of tcanetpp.
  *
  * tcanetpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcanetpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcanetpp.  
  * If not, see <http://www.gnu.org/licenses/>.
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

