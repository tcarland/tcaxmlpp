/**
  *  RandomPrefix.cpp
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
#define _TCANETPP_RANDOMPREFIX_CPP_

extern "C" {
# include <time.h>
# include <stdlib.h>
# include <math.h>
#ifndef WIN32
# include <unistd.h>
#endif
}

#include "RandomPrefix.h"
#include "CidrUtils.h"


namespace tcanetpp {


bool
Random::_seeded = false;


uint32_t
Random::RandomValue ( double range )
{
    if ( ! _seeded ) {
	tcanet_seed();
	_seeded = true;
    }
    return tcanet_randomValue(range);
}


Prefix*
Random::RandomPrefix ( Prefix* agg )
{
    Prefix * p;

    p =  new Prefix(Random::RandomPrefix(agg->getPrefix(), 
		    agg->getPrefixLen()), 24);

    return p;
}


ipv4addr_t
Random::RandomPrefix ( ipv4addr_t agg, uint8_t mb )
{
    int         blksz    = 0;
    int         subval   = 0;
    uint8_t     subnet   = 0;
    uint8_t     pos      = 0;
    uint8_t    *ptr;
    uint8_t     octets[4];
    ipv4addr_t  addr     = 0;

    if ( ! _seeded ) {
	tcanet_seed();
	_seeded = true;
    }

    blksz = CidrUtils::GetCidrRange(mb, &pos);

    if ( pos > 0 ) // adjust for octet array
	pos--;

    ptr   = (uint8_t*) &agg;

    for ( int i = 0; i < 4; i++, ptr++ )
	octets[i] = (*(uint8_t*)ptr);

    subnet = octets[pos];
    subval = (int) tcanet_randomValue((double)blksz);

    octets[pos] = (subnet + (subval - 1));
    ptr = &octets[0];

    addr = *(ipv4addr_t*)ptr;
    return addr;
}


} // namespace

// _TCANETPP_RANDOMPREFIX_CPP_

