/**
  * @file AddrInfo.cpp
  *
  * For handling struct addrinfo and getaddrinfo/getnameinfo
  *
  * Copyright (c) 2010 Timothy Charlton Arland
  * @author tcarland@gmail.com
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
#define _TCANETPP_ADDRINFO_CPP_

#include "AddrInfo.h"
#include "StringUtils.h"
#include "Socket.h"


namespace tcanetpp {


// ----------------------------------------------------------------------
/*
   struct addrinfo {
       int              ai_flags;
       int              ai_family;
       int              ai_socktype;
       int              ai_protocol;
       size_t           ai_addrlen;
       struct sockaddr *ai_addr;
       char            *ai_canonname;
       struct addrinfo *ai_next;
   };
*/
// ----------------------------------------------------------------------

AddrInfo::AddrInfoFactory AddrInfo::factory;

AddrInfo*
AddrInfo::AddrInfoFactory::operator() ( struct addrinfo * ai )
{
    return(new AddrInfo(ai));
}


// ----------------------------------------------------------------------

std::string AddrInfo::ai_error = " -- ";


AddrInfo::AddrInfo()
    : _ai(NULL),
      _nxt(NULL)
{}

AddrInfo::AddrInfo ( const std::string & name, const std::string & service )
    : _ai(NULL),
      _nxt(NULL)
{
    _err = AddrInfo::GetAddrInfo(name, service, NULL, &_ai);
}

AddrInfo::AddrInfo ( struct addrinfo * ai )
    : _ai(ai),
      _nxt(NULL)
{
}

AddrInfo::~AddrInfo()
{
    if ( _ai )
        ::freeaddrinfo(_ai);
}

// ----------------------------------------------------------------------

AddrInfo::AddrInfo ( const AddrInfo & ai )
    : _ai(ai._ai),
      _nxt(ai._nxt)
{
}

AddrInfo&
AddrInfo::operator= ( const AddrInfo & ai )
{
    if ( this == &ai )
        return *this;
    if ( _ai && _ai != ai._ai )
        ::freeaddrinfo(_ai);

    _ai  = ai._ai;
    _nxt = ai._nxt;

    return *this;
}

// ----------------------------------------------------------------------

bool
AddrInfo::isValid() const
{
    if ( _ai == NULL )
        return false;
    return true;
}

addrinfo*
AddrInfo::begin()
{
    _nxt = _ai;
    return _nxt;
}

addrinfo*
AddrInfo::next()
{
    if ( _nxt != NULL )
        _nxt = _nxt->ai_next;

    return _nxt;
}

/** Returns a pointer to the underlying struct addrinfo */
addrinfo*
AddrInfo::get()
{
    return _ai;
}

/** Returns the IP address of the underlying addrinfo
  *  as a struct sockaddr .
 **/
sockaddr_t*
AddrInfo::getAddr()
{
    if ( _ai )
        return((sockaddr_t*) _ai->ai_addr);
    return NULL;
}

/** Returns the currently configured flags for the
  * underlying struct addrinfo.
 **/
int
AddrInfo::getFlags() const
{
    if ( _ai )
        return _ai->ai_flags;
    return 0;
}

/** Set the flags for the addrinfo. */
bool
AddrInfo::setFlags ( int flags )
{
    if ( _ai ) {
        _ai->ai_flags = flags;
        return true;
    }
    return false;
}

/** Returns the address family value */
int
AddrInfo::getFamily() const
{
    if ( _ai )
        return _ai->ai_family;
    return 0;
}

/** Sets the address family of the underlying addrinfo object */
bool
AddrInfo::setFamily ( int family )
{
    if ( _ai ) {
        _ai->ai_family = family;
        return true;
    }
    return false;
}

int
AddrInfo::getSocktype() const
{
    if ( _ai )
        return _ai->ai_socktype;
    return 0;
}

bool
AddrInfo::setSocktype ( int type )
{
    if ( _ai ) {
        _ai->ai_socktype = type;
        return true;
    }
    return false;
}

/** Returns the protocol set within the struct sockaddr. */
int
AddrInfo::getProtocol() const
{
    if ( _ai )
        return _ai->ai_protocol;
    return 0;
}

/** Sets the addrinfo protocol */
bool
AddrInfo::setProtocol ( int proto )
{
    if ( _ai ) {
        _ai->ai_protocol = proto;
        return true;
    }
    return false;
}

/** Returns the ai_addrlen of the underlying struct addrinfo */
size_t
AddrInfo::getAddrLen() const
{
    if ( this->isValid() )
        return _ai->ai_addrlen;
    return 0;
}

/** Returns the canonical name if applicable, or NULL if not set */
char*
AddrInfo::getCanonName() const
{
    if ( this->isValid() )
        return _ai->ai_canonname;
    return NULL;
}

int
AddrInfo::getError() const
{
    return _err;
}

// ----------------------------------------------------------------------
// static factory methods

/**  Static functions that performs getaddrinfo and returns a pointer to
  *  a newly allocated AddrInfo object.
  * @{
 **/
AddrInfo*
AddrInfo::GetAddrInfo ( const std::string & host, uint16_t port, const addrinfo * hints )
{
    std::string svc = StringUtils::toString<uint16_t>(port);

    return AddrInfo::GetAddrInfo(host, svc, hints);
}

AddrInfo*
AddrInfo::GetAddrInfo ( const std::string & host, const std::string & svc, const addrinfo * hints )
{
    AddrInfo  * ai = NULL;
    addrinfo  * res;
    int         err = 0;

    if ( (err = AddrInfo::GetAddrInfo(host, svc, hints, &res)) == 0 )
        ai = AddrInfo::factory(res);
    else
        ai_error = (std::string) ::gai_strerror(err);

    return ai;
}
/*@}*/


/**  Static wrapper function to getaddrinfo. Returns 0 on success
  *  or non-zero error code.
 **/
/*@{*/
int
AddrInfo::GetAddrInfo ( const std::string & host,
                        const addrinfo    * hints,
                        addrinfo         ** res )
{
    std::string svc = "";
    return AddrInfo::GetAddrInfo(host, svc, hints, res);
}


int
AddrInfo::GetAddrInfo ( const std::string & host, 
                        const std::string & svc,
                        const addrinfo    * hints,
                        addrinfo         ** res )
{
    const char * hostname = NULL;
    const char * service  = NULL;

    if ( ! host.empty() )
        hostname = host.c_str();
    if ( ! svc.empty() )
        service  = svc.c_str();

    return(::getaddrinfo(hostname, service, hints, res));
}
/*@}*/


// ----------------------------------------------------------------------

/** Static functions that wrap getnameinfo for reverse lookups.
  * Returns 0 on success or non-zero error-code indicating failure.
 **/
/*@{*/
int
AddrInfo::GetNameInfo ( const ipv4addr_t  & addr,
                        std::string       & result,
                         int                 flags)
{
    sockaddr_in  sa;
    socklen_t    salen;

    salen  = sizeof(sa);
    ::memset(&sa, 0, salen);

    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = addr;

    return AddrInfo::GetNameInfo((const sockaddr*) &sa, salen, result, flags);
}


int
AddrInfo::GetNameInfo ( const ipv6addr_t  & addr,
                        std::string       & result,
                        int                 flags )
{
    sockaddr_in6  sa;
    socklen_t     salen;

    salen  = sizeof(sa);
    ::memset(&sa, 0, salen);

    sa.sin6_family = AF_INET6;
    sa.sin6_addr   = addr.toAddr();

    return AddrInfo::GetNameInfo((const sockaddr*) &sa, salen, result, flags);
}


int
AddrInfo::GetNameInfo ( const sockaddr    * sock,
                        socklen_t           socklen,
                        std::string       & result,
                        int                 flags )
{
    char   host[TCANET_MEDSTRLINE];
    char   serv[TCANET_MEDSTRLINE];
    size_t len = TCANET_MEDSTRLINE;
    int    res = 0;

    res   = ::getnameinfo(sock, socklen, &host[0], len, &serv[0], len, flags);

    if ( res == 0 )
        result.assign(host);

    return res;
}
/*@}*/

// ----------------------------------------------------------------------

std::string
AddrInfo::GetHostName()
{
    std::string  host;
    ipv6addr_t   lo  = in6addr_loopback;
    ipv4addr_t   lo4 = IPV4ADDR_LOOPBACK;
    int  r;

    r = AddrInfo::GetNameInfo(lo, host, 0);

    if ( r == 0 )
        return host;

    r = AddrInfo::GetNameInfo(lo4, host, 0);

    return host;
}

std::string
AddrInfo::GetHostName ( const ipv4addr_t & addr )
{
    std::string host;
    AddrInfo::GetNameInfo(addr, host, 0);
    return host;
}


/**  First attempts to convert the provided string
  *  to an ipv4 address, or upon failure, will
  *  perform a name lookup via getaddrinfo and
  *  return the first valid ipv4 address returned.
 */
ipv4addr_t
AddrInfo::GetHostAddr ( const std::string & host )
{
    ipv4addr_t  addr = 0;

    if ( IpAddr::pton(host, addr) == 1 )
        return addr;

    AddrInfo * ai;
    struct addrinfo  *res;

    ai  = AddrInfo::GetAddrInfo(host, "");

    if ( ai == NULL )
        return addr;

    res = ai->begin();
    while ( res ) {
        if ( res->ai_family == AF_INET ) {
            sockaddr_in * sa = (sockaddr_in*) res->ai_addr;
            addr = sa->sin_addr.s_addr;
            break;
        }
        res = ai->next();
    }
    delete ai;

    return addr;
}

int
AddrInfo::GetAddrList ( const std::string & host, IpAddrList & v )
{
    AddrInfo * ai = AddrInfo::GetAddrInfo(host, "");

    if ( ai == NULL )
        return 0;

    struct addrinfo * res;

    for ( res = ai->begin(); res != NULL; res = ai->next() )
    {
        IpAddr addr(res->ai_addr);
        v.push_back(addr);
    }

    return 1;
}

// ----------------------------------------------------------------------

addrinfo
AddrInfo::GetTCPServerHints()
{
    struct addrinfo  hints;

    ::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_STREAM;
    hints.ai_flags     = AI_PASSIVE;

    return hints;
}

addrinfo
AddrInfo::GetUDPServerHints()
{
    struct addrinfo  hints;

    ::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_DGRAM;
    hints.ai_flags     = AI_PASSIVE;

    return hints;
}

addrinfo
AddrInfo::GetTCPClientHints()
{
    struct addrinfo  hints;

    ::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_STREAM;

    return hints;
}

addrinfo
AddrInfo::GetUDPClientHints()
{
    struct addrinfo  hints;

    ::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_DGRAM;

    return hints;
}

// ----------------------------------------------------------------------

std::string
AddrInfo::GetErrorStr()
{
    return AddrInfo::ai_error;
}

// ----------------------------------------------------------------------

} // namespace


// _TCANETPP_ADDRINFO_CPP_
