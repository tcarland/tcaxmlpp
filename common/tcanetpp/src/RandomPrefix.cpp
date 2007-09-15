#define _TCANETPP_RANDOMPREFIX_CPP_

extern "C" {
# include <time.h>
# include <unistd.h>
# include <stdlib.h>
# include <math.h>
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

    blksz = CidrUtils::getCidrRange(mb, &pos);

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
