
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
    return new AddrInfo(ai);
}


// ----------------------------------------------------------------------

int AddrInfo::ai_error = 0;


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


AddrInfo::~AddrInfo()
{
    if ( _ai )
        ::freeaddrinfo(_ai);
}

// ----------------------------------------------------------------------

AddrInfo::AddrInfo ( struct addrinfo * ai )
    : _ai(ai),
      _nxt(NULL)
{
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
    if ( _nxt == NULL )
        _nxt = _ai;
    else
        _nxt = _nxt->ai_next;
    return _nxt;
}

addrinfo*
AddrInfo::get()
{
    return _ai;
}


int
AddrInfo::getFlags() const
{
    if ( _ai )
        return _ai->ai_flags;
    return 0;
}

bool
AddrInfo::setFlags ( int flags )
{
    if ( _ai ) {
        _ai->ai_flags = flags;
        return true;
    }
    return false;
}

int
AddrInfo::getFamily() const
{
    if ( _ai )
        return _ai->ai_family;
    return 0;
}

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

int
AddrInfo::getProtocol() const
{
    if ( _ai )
        return _ai->ai_protocol;
    return 0;
}

bool
AddrInfo::setProtocol ( int proto )
{
    if ( _ai ) {
        _ai->ai_protocol = proto;
        return true;
    }
    return false;
}

size_t
AddrInfo::getAddrlen() const
{
    if ( this->isValid() )
        return _ai->ai_addrlen;
    return 0;
}

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


AddrInfo*
AddrInfo::GetAddrInfo ( const std::string & host, const std::string & svc )
{
    AddrInfo * ai = NULL;
    addrinfo * res, hints;

    hints = AddrInfo::GetTCPClientHints();

    if ( (ai_error = AddrInfo::GetAddrInfo(host, svc, &hints, &res)) == 0 )
        ai = AddrInfo::factory(res);

    return ai;
}


AddrInfo*
AddrInfo::GetAddrInfo ( const std::string & host, uint16_t port )
{
    std::string svc = StringUtils::toString<uint16_t>(port);

    return AddrInfo::GetAddrInfo(host, svc);
}


int
AddrInfo::GetAddrInfo ( const std::string & host,
                        const addrinfo    * hints,
                        addrinfo         ** res )
{
    return(::getaddrinfo(host.c_str(), NULL, hints, res));
}


int
AddrInfo::GetAddrInfo ( const std::string & host,
                        const std::string & svc,
                        const addrinfo    * hints,
                        addrinfo         ** res )
{
    return(::getaddrinfo(host.c_str(), svc.c_str(), hints, res));
}



// ----------------------------------------------------------------------


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


// ----------------------------------------------------------------------


addrinfo
AddrInfo::GetTCPServerHints()
{
    struct addrinfo  hints;

    ::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCKET_TCP;
    hints.ai_flags     = AI_PASSIVE;

    return hints;
}

addrinfo
AddrInfo::GetUDPServerHints()
{
    struct addrinfo  hints;

    ::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCKET_UDP;
    hints.ai_flags     = AI_PASSIVE;

    return hints;
}

addrinfo
AddrInfo::GetTCPClientHints()
{
    struct addrinfo  hints;

    ::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCKET_TCP;
    hints.ai_flags     = AI_CANONNAME;
    hints.ai_protocol  = 0;

    return hints;
}

addrinfo
AddrInfo::GetUDPClientHints()
{
    struct addrinfo  hints;

    ::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCKET_UDP;
    hints.ai_flags     = AI_CANONNAME;
    hints.ai_protocol  = 0;

    return hints;
}


} // namespace


// _TCANETPP_ADDRINFO_CPP_
