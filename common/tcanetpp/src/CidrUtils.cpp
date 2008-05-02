/**  
  *  Network ip address/prefix/cidr manipulation methods
  *
  *  @Author  tcarland@gmail.com
  *  @Versoin 5.0
 **/
#define _TCANETPP_CIDRUTILS_CPP_

#include <math.h>
#include <vector>

#ifndef WIN32
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netdb.h>
#endif

#include "CidrUtils.h"
#include "Socket.h"
#include "StringUtils.h"


namespace tcanetpp {


//-------------------------------------------------------------------//
 
bool
CidrUtils::isBasePrefix ( ipv4addr_t addr, uint8_t mb )
{
    return(CidrUtils::toBasePrefix(addr, mb) == addr );
}


//-------------------------------------------------------------------//

ipv4addr_t
CidrUtils::toBasePrefix ( ipv4addr_t addr, uint8_t mb )
{
    uint32_t  mask;

    if (  mb > MAXMASKLEN )
	return 0;

    mask = 0xffffffff;
    mask = mask >> (32 - mb) << (32 - mb);

    addr &= htonl(mask);

    return addr;
}

//-------------------------------------------------------------------//

std::string
CidrUtils::toString ( ipv4addr_t addr )
{
    return CidrUtils::ntop(addr);
}


std::string
CidrUtils::toCidrString ( ipv4addr_t addr, uint8_t mb )
{
    char   cidr[INET_CIDRSTRLEN];

    std::string cidrStr = CidrUtils::ntop(addr);
    sprintf(cidr, "%s%s%u", cidrStr.c_str(), "/", mb);

    return((std::string)cidr);
}


std::string
CidrUtils::toCidrString ( const Prefix & p )
{
    return CidrUtils::toCidrString(p.getPrefix(), p.getPrefixLen());
}

//-------------------------------------------------------------------//

int
CidrUtils::StringToAddr ( const std::string & addrStr, ipv4addr_t & addr )
{
    int rs = 0;

#   ifdef WIN32
    addr = CidrUtils::pton(addrStr);
    if ( addr > 0 )
        rs   = 1;
#   else
    rs = inet_pton(AF_INET, addrStr.c_str(), &addr);
#   endif

    return rs;
}

//-------------------------------------------------------------------//

Prefix
CidrUtils::StringToCidr ( const std::string & cidrStr )
{
    Prefix      prefix;
    std::string addrstr = "";
    ipv4addr_t  addr    = 0;
    uint8_t     mb      = 0;
    
    std::vector<std::string>  stok;
    
    StringUtils::split(cidrStr, '/', std::back_inserter(stok));
    
    if ( stok.size() >= 1 )
        addrstr = stok[0];

    if ( CidrUtils::StringToAddr(addrstr, addr) <= 0 )
	return prefix;

    if ( stok.size() == 2 )
	mb = (uint8_t) StringUtils::fromString<uint8_t>(stok[1]);

    if ( mb <= 0 || mb > 32 )
	mb = 32;

    return(Prefix(addr, mb));
}

//-------------------------------------------------------------------//

ipv4addr_t
CidrUtils::bitsToMask ( uint8_t mb )
{
    ipv4addr_t  mask = 0xffffffff;
    mask = mask >> (32 - mb) << (32 - mb);
    return mask;
}

//-------------------------------------------------------------------//

int
CidrUtils::getCidrRange ( uint8_t mb, uint8_t * subnet_pos )
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
CidrUtils::getCidrRange ( uint8_t mb )
{
    return CidrUtils::getCidrRange(mb, NULL);
}

//-------------------------------------------------------------------//

uint8_t
CidrUtils::subnetValue ( ipv4addr_t addr, uint8_t subnet_pos )
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

//-------------------------------------------------------------------//

bool
CidrUtils::deAggregate ( Prefix & p, uint8_t mb, std::vector<Prefix> &v )
{
    uint32_t base;
    uint8_t  octets[4], *ptr;
    int      indxA, indxB, num;
    bool     big = false;

    if ( p.getPrefixLen() > mb || 
	! CidrUtils::isBasePrefix(p.getPrefix(), p.getPrefixLen()) )
	return false;

    num = (int) pow(2, (mb - p.getPrefixLen()));

    indxA = ( 3 - ((32 - p.getPrefixLen()) / 8) );
    indxB = ( 3 - ((32 - mb) / 8) );

    if ( indxA == 0 ) {
        indxA = 1;
        big = true;
    }
    
    base = p.getPrefix();
    ptr  = (uint8_t*) &base;
    for ( int i = 0; i < 4; i++, ptr++ )
	octets[i] = (*(uint8_t*)ptr);

    ptr = &octets[0];

    while ( num != 0 ) {
        Prefix p = Prefix( (*(ipv4addr_t*)ptr), mb );
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

//-------------------------------------------------------------------//

bool
CidrUtils::matchCidr ( Prefix & p, ipv4addr_t addr )
{
    return(p.getPrefix() == CidrUtils::toBasePrefix(addr, p.getPrefixLen()));
}


//-------------------------------------------------------------------//

std::string
CidrUtils::ntop ( ipv4addr_t addr )
{
    char   ip[INET_ADDRSTRLEN];
    std::string ipstr;

#   ifdef WIN32
    inaddr_t  wip;
    wip.s_addr = addr;
    ::strncpy(ip, ::inet_ntoa(wip), INET_ADDRSTRLEN);
#   else
    ::inet_ntop(AF_INET, &addr, ip, sizeof(ip));
#   endif

    ipstr.assign(ip);
    
    return(ipstr);
}

ipv4addr_t
CidrUtils::pton ( const std::string & ipstr )
{
    ipv4addr_t addr = 0;

#   ifdef WIN32
    addr = (unsigned long) inet_addr(ipstr.c_str());
    if ( addr == INADDR_NONE )
        addr = 0;
#   else
    ::inet_pton(AF_INET, ipstr.c_str(), &addr);
#   endif

    return addr;
}

//-------------------------------------------------------------------//

std::string
CidrUtils::getHostName()
{
    char         hstr[MAXSTRLINE];
    std::string  hostname;

    if ( ::gethostname(hstr, (size_t) MAXSTRLINE) < 0 )
        return hostname;

    hostname = hstr;

    return hostname;
}

std::string
CidrUtils::getHostName ( ipv4addr_t addr )
{
    std::string       hostname;
    struct  hostent  *hp = NULL;
        
    if ( (hp = ::gethostbyaddr((char*)&addr, sizeof(addr), AF_INET)) == NULL )
        return hostname;

    hostname = hp->h_name;

    return hostname;
}


ipv4addr_t
CidrUtils::getHostAddr()
{
    ipv4addr_t       addr = 0;
    struct hostent * hp   = NULL;
    std::string      host;

    host = CidrUtils::getHostName();

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
CidrUtils::getHostAddr ( const std::string & host )
{
    struct hostent * hp;
    ipv4addr_t       addr = 0;

    if ( CidrUtils::StringToAddr(host, addr) )
        return addr;

    hp   = ::gethostbyname(host.c_str());

    if ( ! hp )
        return 0;
    
    addr = *((uint32_t*)hp->h_addr);

    return addr;
}


void
CidrUtils::getHostAddrList ( const std::string & host, AddrList & addrlist )
{
    struct hostent  *hp;
    char           **pptr;

    if ( (hp = ::gethostbyname(host.c_str())) == NULL )
        return;

    switch ( hp->h_addrtype ) {
         case AF_INET:
             pptr = hp->h_addr_list;
             for ( ; *pptr != NULL; pptr++ )
                 addrlist.push_back(*((ipv4addr_t*) *pptr));
             break;
//#ifdef AF_INET6
        //case AF_INET6:
//#endif
          default:
             return;
    }

    return;
}

//-------------------------------------------------------------------//

bool
CidrUtils::isLoopback ( ipv4addr_t addr )
{
    return(addr == (CidrUtils::pton(std::string("127.0.0.1"))));
}

//-------------------------------------------------------------------//

} //namespace

//  _TCANETPP_CIDR_CPP_ 
