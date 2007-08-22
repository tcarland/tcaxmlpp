/**  
  *  Network ip address/prefix/cidr manipulation methods
  *
  *  @Author  tcarland@gmail.com
  *  @Versoin 0.1
 **/
#define _TCANETPP_CIDR_CPP_

#include <math.h>
#include <cstdio>

#include "Cidr.h"
#include "Socket.h"
#include "StringUtils.h"
#include "StringTokenizer.hpp"


namespace tcanetpp {


bool
Cidr::isBasePrefix ( ipv4addr_t addr, uint8_t mb )
{
    return(Cidr::toBasePrefix(addr, mb) == addr );
}


ipv4addr_t
Cidr::toBasePrefix ( ipv4addr_t addr, uint8_t mb )
{
    uint32_t mask;

    if (  mb > MAX_MASKLEN )
	return 0;

    mask = 0xffffffff;
    mask = mask >> (32 - mb) << (32 - mb);

    addr &= htonl(mask);

    return addr;
}


std::string
Cidr::toString ( ipv4addr_t addr )
{
    return Cidr::ntop(addr);
}


std::string
Cidr::toCidrString ( ipv4addr_t addr, uint8_t mb )
{
    char   cidr[CIDR_ADDRSTRLEN];

    std::string cidrStr = Cidr::toString(addr);
    sprintf(cidr, "%s%s%d", cidrStr.c_str(), "/", mb);

    return((std::string)cidr);
}


std::string
Cidr::toCidrString ( const Prefix & p )
{
    return Cidr::toCidrString(p.getPrefix(), p.getPrefixLen());
}


int
Cidr::StringToAddr ( const std::string & addrStr, ipv4addr_t & addr )
{
    int rs = 0;

#   ifdef WIN32
    addr = Cidr::pton(addrStr);
    if ( addr > 0 )
        rs   = 1;
#   else
    rs = inet_pton(AF_INET, addrStr.c_str(), &addr);
#   endif

    return rs;
}


Prefix
Cidr::StringToCidr ( const std::string & cidrStr )
{
    Prefix      prefix;
    std::string addrstr = "";
    ipv4addr_t  addr    = 0;
    uint8_t     mb      = 0;

    StringTokenizer stok(cidrStr.c_str(), "/");

    if ( stok.size() >= 1 )
	addrstr = stok.nextToken();

    if ( Cidr::StringToAddr(addrstr, addr) <= 0 )
	return prefix;

    if ( stok.hasMoreTokens() )
	mb = (uint8_t) atoi(stok.nextToken().c_str());

    if ( mb <= 0 || mb > 32 )
	mb = 32;

    return(Prefix(addr, mb));
}


uint8_t
Cidr::getSubnetHostBits ( uint8_t mb )
{
    uint8_t hostbits = 0;

    if ( mb > MAX_MASKLEN )
	return hostbits;

    hostbits = MAX_MASKLEN - mb;

    while ( hostbits > 8 )
	hostbits -= 8;

    return hostbits;
}


uint8_t
Cidr::getHostBits ( ipv4addr_t addr )
{
    uint8_t  bits = 0;
    uint8_t *ptr;

    ptr = (uint8_t*) &addr;

    for ( int i = 0; i < 32; i++, ptr++ ) {
        if ( *ptr > 0 )
            bits++;
    }

    return bits;
}


ipv4addr_t
Cidr::bitsToMask ( uint8_t mb )
{
    ipv4addr_t   mask = 0xffffffff;
    mask = mask >> (32 - mb) << (32 - mb);
    return mask;
}


int
Cidr::getCidrRange ( uint8_t mb, uint8_t* subnet_pos )
{
    uint8_t  pos = 1;

    while ( mb >= 8 ) {
        mb -= 8;
        pos++;
    }

    if ( subnet_pos )
        *subnet_pos = pos;

    return (int) pow(2, (8 - mb));
}


int
Cidr::getCidrRange ( uint8_t mb )
{
    return Cidr::getCidrRange(mb, NULL);
}


uint8_t
Cidr::subnetValue ( ipv4addr_t addr, uint8_t subnet_pos )
{
    uint8_t   octets[4];
    uint8_t  *ptr;

    if ( subnet_pos > 4 )
        return 0;

    ptr  = (uint8_t*) &addr;

    for ( int i = 0; i < 4; i++, ptr++ )
	octets[i] = (*(uint8_t*)ptr);

    return octets[subnet_pos - 1];
}


/*  Rather crazy function to de-aggregate cidr addr's 
 *  
 *  @param p  is the given CIDR prefix to deagg.
 *  @param mb is the desired masklen to deagg to
 *  @param v  is the vector where results will be put.
 */
bool
Cidr::deAggregate ( Prefix & p, uint8_t mb, std::vector<Prefix> &v )
{
    uint32_t base;
    uint8_t  octets[4], *ptr;
    int      indxA, indxB, num;
    bool     big = false;

    // you would think we could trust the masklen, but
    // we check anyways, and we validate the prefix 
    if ( p.getPrefixLen() > mb || 
	! Cidr::isBasePrefix(p.getPrefix(), p.getPrefixLen()) )
	return false;

    num = (int) pow(2, (mb - p.getPrefixLen()));

    // get just two indexes, which could break when
    // expanding nets less than a /8.
    indxA = ( 3 - ((32 - p.getPrefixLen()) / 8) );
    indxB = ( 3 - ((32 - mb) / 8) );

    if ( indxA == 0 ) {
        indxA = 1;
        big = true;
    }
    //printf("indxA %d, indxB %d\n", indxA, indxB);
    
    base = p.getPrefix();
    ptr  = (uint8_t*) &base;
    for ( int i = 0; i < 4; i++, ptr++ )
	octets[i] = (*(uint8_t*)ptr);

    ptr = &octets[0];

    while ( num != 0 ) {
        Prefix p = Prefix( (*(ipv4addr_t*)ptr), mb );
	//printf("adding %s\n", Cidr::toCidrString(p).c_str());
	v.push_back(p);
	octets[indxB] = octets[indxB]++;
	if ( octets[indxB] == 0 ) {
	    octets[indxA] = octets[indxA]++;
            if ( big && octets[indxA] == 0 )
                octets[0] = octets[0]++;
        }
	num--;
    }

    return true;
}


bool
Cidr::matchCidr ( Prefix & p, ipv4addr_t addr )
{
    return(p.getPrefix() == Cidr::toBasePrefix(addr, p.getPrefixLen()));
}


std::string
Cidr::ntop ( ipv4addr_t addr )
{
    char   ip[INET_ADDRSTRLEN];

#ifdef WIN32
    inaddr_t  wip;
    wip.s_addr = addr;
    strncpy(ip, inet_ntoa(wip), INET_ADDRSTRLEN);
#else
    inet_ntop(AF_INET, &addr, ip, sizeof(ip));
#endif

    return((std::string) ip);
}


ipv4addr_t
Cidr::pton ( const std::string & ipstr )
{
    ipv4addr_t addr = 0;

#ifdef WIN32
    addr = (unsigned long) inet_addr(ipstr.c_str());
    if ( addr == INADDR_NONE )
        addr = 0;
#else
    inet_pton(AF_INET, ipstr.c_str(), &addr);
#endif

    return addr;
}


std::string
Cidr::getHostName()
{
    char         hstr[MAXSTRLINE];
    std::string  hostname;

    if ( ::gethostname(hstr, (size_t) MAXSTRLINE) < 0 )
        return hostname;

    hostname = hstr;

    return hostname;
}


std::string
Cidr::getHostName ( ipv4addr_t addr )
{
    std::string       hostname;
    struct  hostent  *hp = NULL;
        
    if ( (hp = ::gethostbyaddr((char*)&addr, sizeof(addr), AF_INET)) == NULL )
        return hostname;

    hostname = hp->h_name;

    return hostname;
}


ipv4addr_t
Cidr::getHostAddr()
{
    ipv4addr_t       addr = 0;
    struct hostent  *hp   = NULL;
    std::string      host;

    host = Cidr::getHostName();

    if ( host.length() == 0 )
        return addr;

    hp = ::gethostbyname(host.c_str());

    if ( !hp )
        return addr;

    if ( hp->h_addrtype == AF_INET )
        addr = *((uint32_t*) hp->h_addr);

    return addr;
}


ipv4addr_t
Cidr::getHostAddr ( const std::string & host )
{
    struct hostent  *hp;
    ipv4addr_t       addr = 0;

    if ( Cidr::StringToAddr(host, addr) )
        return addr;

    hp = ::gethostbyname(host.c_str());

    if ( ! hp )
        return 0;
    
    addr = *((uint32_t*)hp->h_addr);

    return addr;
}


void
Cidr::getHostAddrList ( const std::string & host, AddrList & addrlist )
{
    struct hostent  *hp;
    char           **pptr;

    if ( (hp = ::gethostbyname(host.c_str())) == NULL )
        return;

    switch ( hp->h_addrtype ) {
//#ifdef AF_INET6
        //case AF_INET6:
//#endif
         case AF_INET:
             pptr = hp->h_addr_list;
             for ( ; *pptr != NULL; pptr++ )
                 addrlist.push_back(*((uint32_t*) *pptr));
             break;
          default:
             return;
    }

    return;
}


bool
Cidr::isLoopback ( ipv4addr_t addr )
{
    return(addr == (Cidr::pton(std::string("127.0.0.1"))));
}


} //namespace


//  _TCANETPP_CIDR_CPP_ 
