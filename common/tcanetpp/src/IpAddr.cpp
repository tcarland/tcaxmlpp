/**
  * @file IpAddr.cpp
  *
  *   Defines various IP headers.
  * These are defined within this library to avoid having to
  * use platform specific defines for various headers. This also
  * means we avoid the issue of some fields varying slightly in
  * convention across platforms.
  *
  * Copyright (c) 2010 Timothy Charlton Arland
  * @author tca@charltontechnology.net
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
#define _TCANETPP_IPADDR_CPP_

#include <math.h>
#include <string.h>
#include <cstdio>

#ifndef WIN32
# include <arpa/inet.h>
# include <netinet/ether.h>
#endif

#include "IpAddr.h"
#include "tcanetpp_random.h"

#include "AddrInfo.h"

namespace tcanetpp {


IpAddr::IpAddr()
    : _addr(0, 0),
      _mb(0),
      _ipv4only(false)
{}


IpAddr::IpAddr ( const ipv4addr_t & addr, uint8_t mb )
    : _addr(0, 0),
      _mb(mb),
      _ipv4only(true)
{
    this->setIpAddr(addr);
}


IpAddr::IpAddr ( const ipv6addr_t & addr, uint8_t mb )
    : _addr(addr),
      _mb(mb),
      _ipv4only(false)
{}

IpAddr::IpAddr ( const IpAddr & ipaddr )
    : _addr(ipaddr.getAddr()),
      _mb(ipaddr.getPrefixLen()),
      _ipv4only(ipaddr.ipv4Only())
{}

IpAddr::~IpAddr()
{}


bool
IpAddr::operator== ( const IpAddr & ipaddr ) const
{
    return(_addr == ipaddr.getAddr());
}

bool
IpAddr::operator< ( const IpAddr & ipaddr ) const
{
    return(_addr < ipaddr.getAddr());
}


void
IpAddr::setIpAddr ( const ipv4addr_t & addr )
{
    uint32_t *  oct;
    
    oct    = ((uint32_t*) &_addr);
    oct[3] = addr;

    return;
}


bool
IpAddr::ipv4Only() const
{
    return this->_ipv4only;
}

void
IpAddr::ipv4only ( bool ipv4 )
{
    _ipv4only = ipv4;
}

ipv6addr_t
IpAddr::getAddr() const
{
    return this->getAddr6();
}

ipv4addr_t
IpAddr::getAddr4() const
{
    ipv4addr_t addr;
    uint32_t * oct;

    oct  = ((uint32_t*) &_addr);
    addr = oct[3];

    return addr;
}

ipv6addr_t
IpAddr::getAddr6() const
{
    return _addr;
}

ipv4addr_t
IpAddr::getPrefix() const
{
    return this->getAddr4();
}

ipv6addr_t
IpAddr::getPrefix6() const
{
    return this->getAddr6();
}

uint8_t
IpAddr::getPrefixLen() const
{
    return this->_mb;
}

sockaddr_t
IpAddr::getSockAddr()
{
    sockaddr_t  ss;
    socklen_t   sslen;

    sslen = sizeof(ss);
    ::memset(&ss, 0, sslen);

    if ( this->ipv4only() )
    {
        sockaddr_in * sock    = (sockaddr_in*) &ss;
        sock->sin_family      = AF_INET;
        sock->sin_addr.s_addr = this->getAddr4();
    }
    else
    {
        sockaddr_in6 * sock = (sockaddr_in6*) &ss;
        sock->sin6_family   = AF_INET6;
        sock->sin6_addr     = *((in6addr_t*) &_addr);
    }

    return ss;
}

std::string
IpAddr::toString() const
{
    if ( _ipv4only ) {
        ipv4addr_t addr = this->getAddr4();
        return IpAddr::ntop(addr);
    }

    std::string  ipstr;
    AddrInfo::GetNameInfo(_addr, ipstr, NI_NUMERICHOST);

    return ipstr;
}

bool
IpAddr::isLoopback() const
{
    if ( _ipv4only ) {
        ipv4addr_t addr = this->getAddr4();
        return IpAddr::IsLoopback(addr);
    }

    return IpAddr::IsLoopback(_addr);
}


bool
IpAddr::IsLoopback ( const ipv4addr_t & addr )
{
    return(addr == IPV4ADDR_LOOPBACK);
}

bool
IpAddr::IsLoopback ( const ipv6addr_t & addr )
{
    return(addr == in6addr_loopback);
}

//-------------------------------------------------------------------//

std::string
IpAddr::ntop ( const ipv4addr_t & addr )
{
    const char * dst;
    char         ip[INET_ADDRSTRLEN];
    std::string  ipstr;

#   ifdef WIN32
    inaddr_t   wip;
    wip.s_addr = addr;
    ::strncpy(ip, ::inet_ntoa(wip), INET_ADDRSTRLEN);
#   else
    dst = ::inet_ntop(AF_INET, &addr, ip, sizeof(ip));
#   endif

    if ( dst != NULL )
        ipstr.assign(ip);
    
    return ipstr;
}

std::string
IpAddr::ntop ( const ipv6addr_t & addr )
{
    const char * dst = NULL;
    char         ip[INET6_ADDRSTRLEN];
    std::string  ipstr;

    dst = ::inet_ntop(AF_INET6, &addr, ip, INET6_ADDRSTRLEN);

    if ( dst )
        ipstr.assign(ip);

    return ipstr;
}

//-------------------------------------------------------------------//

int
IpAddr::pton ( const std::string & ipstr, ipv4addr_t & addr )
{
    int rs = 0;

#   ifdef WIN32
    addr = (unsigned long) inet_addr(ipstr.c_str());
#   else
    rs = ::inet_pton(AF_INET, ipstr.c_str(), &addr);
#   endif

    return rs;
}

int
IpAddr::pton ( const std::string & ipstr, ipv6addr_t & addr )
{
    return( ::inet_pton(AF_INET6, ipstr.c_str(), &addr) );
}


//-------------------------------------------------------------------//

std::string
IpAddr::ether_ntop ( const ethaddr_t * ethaddr )
{
    std::string  ethstr;
    char hwaddr[18];
    
    if ( ethaddr == NULL )
        return ethstr;

    ::snprintf(hwaddr, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
        ethaddr->ether_octet[0], ethaddr->ether_octet[1],
        ethaddr->ether_octet[2], ethaddr->ether_octet[3],
        ethaddr->ether_octet[4], ethaddr->ether_octet[5]);

    ethstr.assign(hwaddr);

    return ethstr;
}

//-------------------------------------------------------------------//

int
IpAddr::GetCidrRange ( uint8_t mb, uint8_t * subnet_pos )
{
    uint8_t  pos = 1;
    uint8_t  len = 8;

    if ( mb > 32 )
        return 0;

    while ( mb >= len ) {
        mb -= len;
        pos++;
    }

    if ( subnet_pos )
        *subnet_pos = pos;

    return( ::pow((double)2, (double)(len - mb)) );
}

int
IpAddr::GetIpv6Range ( uint8_t mb )
{
    uint8_t  len = 64;

    if ( mb > len )
        return 0;

    return( ::pow((double)2, (double)(len - mb)) );
}

//-------------------------------------------------------------------//

uint32_t
IpAddr::RandomValue ( double range ) 
{
    if ( tcanet_seeded() == 0 )
        tcanet_seed();
    return tcanet_randomValue(range);
}

IpAddr
IpAddr::RandomAddr ( IpAddr & agg )
{
    IpAddr  addr;

    if ( agg.ipv4Only() )
        addr = IpAddr(IpAddr::RandomPrefix(agg.getPrefix(), agg.getPrefixLen()), 24);
    return addr;
}

ipv4addr_t
IpAddr::RandomPrefix ( ipv4addr_t agg, uint8_t masklen )
{
    ipv4addr_t  addr     = 0;
    int         blksz    = 0;
    int         subval   = 0;
    uint8_t     subnet   = 0;
    uint8_t     pos      = 0;
    uint8_t    *ptr;
    uint8_t     octets[4];

    if ( tcanet_seeded() == 0 )
	tcanet_seed();

    blksz = IpAddr::GetCidrRange(masklen, &pos);

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


// _TCANETPP_IPADDR_CPP_

