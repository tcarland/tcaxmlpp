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
#include "AddrInfo.h"
#include "StringUtils.h"
#include "tcanetpp_random.h"


namespace tcanetpp {


//-------------------------------------------------------------------//


IpAddr::IpAddr()
    :  _mb(0)
{
    ::memset(&_saddr, 0, sizeof(sockaddr_t));
}


IpAddr::IpAddr ( const ipv4addr_t & addr, uint8_t mb )
    :  _mb(mb)
{
    struct sockaddr_in * sin;

    ::memset(&_saddr, 0, sizeof(sockaddr_t));

    sin  = (struct sockaddr_in*) &_saddr;
    sin->sin_family      = AF_INET;
    sin->sin_addr.s_addr = addr;
}


IpAddr::IpAddr ( const sockaddr * sa )
    : _mb(0)
{
    sockaddr_t * s = (sockaddr_t*) sa;

    ::memcpy(&_saddr, s, sizeof(sockaddr_t));

    switch ( s->ss_family )
    {
        case AF_INET:
            _mb  = 32;
            break;
        case AF_INET6:
            _mb  = 64;
            break;
        default:
            _mb  = 0;
            break;
    }
}


IpAddr::IpAddr ( const ipv6addr_t & addr, uint8_t mb )
    :  _mb(mb)
{
    struct sockaddr_in6 * sin6;

    ::memset(&_saddr, 0, sizeof(sockaddr_t));

    sin6  = (struct sockaddr_in6*) &_saddr;
    sin6->sin6_family = AF_INET6;
    sin6->sin6_addr   = (*( (struct in6_addr*) &addr ));
}


IpAddr::IpAddr ( const IpAddr & ipaddr )
    :  _mb(ipaddr.getPrefixLen())
{
    ::memcpy(&_saddr, ipaddr.getSockAddr(), sizeof(sockaddr_t));
}


IpAddr::~IpAddr()
{}

//-------------------------------------------------------------------//

bool
IpAddr::operator== ( const IpAddr & ipaddr ) const
{
    return( this->getAddr6() == ipaddr.getAddr6() );
}

bool
IpAddr::operator< ( const IpAddr & ipaddr ) const
{
    return( this->getAddr6() < ipaddr.getAddr6() );
}

//-------------------------------------------------------------------//

bool
IpAddr::ipv4() const
{
    if ( _saddr.ss_family == AF_INET )
        return true;
    return false;
}

bool
IpAddr::ipv6() const
{
    if ( _saddr.ss_family == AF_INET6 )
        return true;
    return false;
}

//-------------------------------------------------------------------//

ipv6addr_t
IpAddr::getAddr() const
{
    return this->getAddr6();
}

ipv4addr_t
IpAddr::getAddr4() const
{
    ipv4addr_t    addr;
    sockaddr_in * sa;

    if ( this->ipv6() )
        return 0;

    sa   = (sockaddr_in*) &_saddr;
    addr = sa->sin_addr.s_addr;

    return addr;
}

ipv6addr_t
IpAddr::getAddr6() const
{
    ipv6addr_t     addr;
    sockaddr_in6 * sa;

    if ( this->ipv4() )
        return addr;

    sa   = (sockaddr_in6*) &_saddr;
    addr = ( *((ipv6addr_t*)&sa->sin6_addr) );

    return addr;
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

//-------------------------------------------------------------------//

sockaddr_t*
IpAddr::getSockAddr()
{
    return &_saddr;
}

const sockaddr_t*
IpAddr::getSockAddr() const
{
    return( (const sockaddr_t*) &_saddr );
}

//-------------------------------------------------------------------//

std::string
IpAddr::toString() const
{
    std::string  ipstr;

    if ( this->ipv4() )
        ipstr = IpAddr::ntop(this->getAddr4());
    else
        AddrInfo::GetNameInfo((const sockaddr*)&_saddr, sizeof(sockaddr_t), ipstr, NI_NUMERICHOST);

    return ipstr;
}

bool
IpAddr::isLoopback() const
{
    if ( this->ipv4() )
        return IpAddr::IsLoopback(this->getAddr4());

    return IpAddr::IsLoopback(this->getAddr6());
}

cidr_t
IpAddr::getCidr() const
{
    cidr_t  cidr;
    cidr.addr = this->getPrefix();
    cidr.mb   = this->getPrefixLen();
    return cidr;
}

//-------------------------------------------------------------------//
//-------------------------------------------------------------------//


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

std::string
IpAddr::ntop ( const sockaddr_t * sock )
{
    std::string  ipstr;
    const char * dst = NULL;
    char         ip[INET6_ADDRSTRLEN];

#   ifdef WIN32
    // getaddrinfo?
#   else

    switch ( sock->ss_family )
    {
        case AF_INET:
            dst = ::inet_ntop(sock->ss_family, &((struct sockaddr_in*) sock)->sin_addr.s_addr, ip, INET6_ADDRSTRLEN);
            break;
        case AF_INET6:
            dst = ::inet_ntop(sock->ss_family, &((struct sockaddr_in6*) sock)->sin6_addr, ip, INET6_ADDRSTRLEN);
            break;
    }
    if ( dst )
        ipstr.assign(ip);

#   endif

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

    if ( agg.ipv4() )
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

    subnet      = octets[pos];
    subval      = (int) tcanet_randomValue( (double)blksz );
    octets[pos] = (subnet + (subval - 1));
    ptr         = &octets[0];
    addr        = *(ipv4addr_t*)ptr;

    return addr;
}

//-------------------------------------------------------------------//

bool
IpAddr::IsBasePrefix ( const ipv4addr_t & pfx, uint8_t mb )
{
    return(IpAddr::ToBasePrefix(pfx, mb) == pfx);
}

ipv4addr_t
IpAddr::ToBasePrefix ( const ipv4addr_t & pfx, uint8_t mb )
{
    ipv4addr_t addr = pfx;
    uint32_t   mask = 0xffffffff;

    if ( mb > MAXMASKLEN )
        return 0;

    mask  = mask >> (MAXMASKLEN - mb) << (MAXMASKLEN - mb);
    addr &= htonl(mask);

    return addr;
}

std::string
IpAddr::ToPrefixStr ( const IpAddr & pfx )
{
    char  pfxstr[INET6_ADDRSTRLEN];
    uint8_t mb = pfx.getPrefixLen();

    std::string prefix = IpAddr::ntop( (const sockaddr_t*) pfx.getSockAddr() );

#   ifdef WIN32
    ::_snprintf(pfxstr, INET6_ADDRSTRLEN, "%s%s%u", prefix.c_str(), "/", mb);
#   else
    ::snprintf(pfxstr, INET6_ADDRSTRLEN, "%s%s%u", prefix.c_str(), "/", mb);
#   endif

    prefix.assign(pfxstr);

    return prefix;
}

int
IpAddr::ToIpAddr ( const std::string & str, IpAddr & ipaddr )
{
    std::string  addrstr;
    ipv4addr_t   addr;
    uint16_t     mb = 0;

    std::string::size_type indx;

    if ( (indx = str.find_first_of('/')) == std::string::npos ) {
        addrstr = str;
        mb      = MAXMASKLEN;
    } else {
        addrstr = str.substr(0, indx);
    }

    if ( IpAddr::pton(addrstr, addr) <= 0 )
        return 0;

    if ( mb == 0 ) {
        addrstr = str.substr(indx+1);
        mb = StringUtils::fromString<uint16_t>(addrstr);
    }

    if ( mb < 1 || mb > 32 )
        mb = 32;

    ipaddr = IpAddr(addr, mb);

    return 1;
}

//-------------------------------------------------------------------//

ipv4addr_t
IpAddr::BitsToMask ( uint8_t mb )
{
    ipv4addr_t  mask = 0xffffffff;

    if ( mb > MAXMASKLEN )
        return mask;

    mask = mask >> (32 - mb) << (32 - mb);

    return mask;
}

uint8_t
IpAddr::SubnetValue ( const ipv4addr_t & addr, uint8_t pos )
{
    const uint8_t * ptr;
    uint8_t   octets[4];

    if ( pos > 4 )
        return 0;

    ptr  = (const uint8_t*) &addr;

    for ( int i = 0; i < 4; i++, ptr++ )
        octets[i] = (*(const uint8_t*)ptr);

    return octets[pos - 1];
}

bool
IpAddr::MatchPrefix ( const IpAddr & pfx, const ipv4addr_t & addr )
{
    return( pfx.getPrefix() == IpAddr::ToBasePrefix(addr, pfx.getPrefixLen()) );
}

//-------------------------------------------------------------------//

bool
IpAddr::DeAggregate ( const IpAddr & pfx, uint8_t mb, IpAddrList & v )
{
    ipv4addr_t  base;
    uint8_t     octets[4];
    uint8_t    *ptr;
    int         A, B, num, x;
    bool        bigA = false;

    if ( pfx.getPrefixLen() > mb ||
            ! IpAddr::IsBasePrefix(pfx.getPrefix(), pfx.getPrefixLen()) )
        return false;

    num = (int) ::pow( (double)2, (double)(mb - pfx.getPrefixLen()) );
    A   = ( 3 - ( (32 - pfx.getPrefixLen()) / 8 ) );
    B   = ( 3 - ( (32 - mb) / 8 ) );

    if ( A == 0 ) {
        A = 1;
        bigA = true;
    }

    base = pfx.getPrefix();
    ptr  = (uint8_t*) &base;

    for ( int i = 0; i < 4; i++, ptr++ )
        octets[i] = ( *(uint8_t*)ptr );

    ptr = &octets[0];

    IpAddr p;
    while ( num > 0 )
    {
        p = IpAddr( *(ipv4addr_t*)ptr, mb );
        v.push_back(p);

        x = octets[B];
        octets[B] = x++;

        if ( octets[B] == 0 )
        {
            x = octets[A];
            octets[A] = x++;

            if ( bigA && octets[A] == 0 ) {
                x = octets[0];
                octets[0] = x++;
            }
        }

        num--;
    }

    return true;
}

//-------------------------------------------------------------------//

} // namespace


// _TCANETPP_IPADDR_CPP_

