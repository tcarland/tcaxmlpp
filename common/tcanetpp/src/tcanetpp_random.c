/**  
  * @file random.c
  *   Random number wrapper
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author  tca@charltontechnology.net
  *
  * @section LICENSE
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

#ifndef WIN32
# include <unistd.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "tcanetpp_random.h"


static int _tcanet_seeded = 0;


static void
TCANET_Seed()
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
TCANET_RandomValue ( double range ) 
{
    long s = abs((long)((range * rand()) / (RAND_MAX + 1.0)));
    return((uint32_t) s);
}



int
tcanet_seeded()
{
    return _tcanet_seeded;
}

void
tcanet_seed()
{
    TCANET_Seed();
    _tcanet_seeded = 1;
}


uint32_t
tcanet_randomValue ( double range )
{
    if ( ! _tcanet_seeded )
	tcanet_seed();
    return TCANET_RandomValue(range);
}


//  _TCANETPP_RANDOM_C_

