/** 
  * @file Socket.h
  *
  *   The base Socket class providing an object-oriented
  * interface to sockets for both Unix and Win32 platforms.
  * 
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author  tcarland@gmail.com
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
#define _TCANETPP_SOCKET_CPP_

#include "Socket.h"

#ifndef WIN32
# include <arpa/inet.h>
# include <netinet/in.h>
# include <poll.h>
# include <fcntl.h>
# include <errno.h>
#endif


#include "StringUtils.h"
#include "IpAddr.h"


namespace tcanetpp {


// ----------------------------------------------------------------------

Socket::SocketFactory Socket::factory;
ipv6addr_t Socket::ipv6addr_any = ipv6addr();

// ----------------------------------------------------------------------

Socket*
Socket::SocketFactory::operator() ( sockfd_t & fd, sockaddr_t & csock,
                                    SocketType type, int protocol )
{
    return new Socket(fd, csock, type, protocol);
}

Socket*
Socket::UdpSocketFactory::operator() ( sockfd_t & fd, sockaddr_t & csock,
                                       SocketType type, int protocol )
{
    return new Socket(fd, _csock, type, protocol);
}

// ----------------------------------------------------------------------

Socket::Socket()
    : _socktype(SOCKTYPE_NONE),
      _proto(0),
      _port(0),
      _bound(false),
      _connected(false),
      _block(false),
      _noUdpClose(false)
{
    Socket::ResetDescriptor(this->_fd);
}

/**  IPV4 constructor for creating a client or server socket utilizing an IPV4
  *  address and port.
 **/
Socket::Socket ( ipv4addr_t ipaddr, uint16_t port, SocketType type, int protocol )
    throw ( SocketException )
    : _ipaddr(ipaddr),
      _socktype(type),
      _proto(protocol),
      _port(port),
      _bound(false),
      _connected(false),
      _block(false),
      _noUdpClose(false)
{
    Socket::ResetDescriptor(this->_fd);
    
    if ( _socktype <= SOCKTYPE_NONE || _socktype > SOCKTYPE_RAW )
        throw SocketException("Socket error: Invalid Socket type");
    if ( _proto < 0 || _proto > 255 )
        throw SocketException("Socket error: Invalid protocol");
    if ( ipaddr == 0 && _socktype < SOCKTYPE_SERVER )
        throw SocketException("Socket error: Invalid IP Address");

    sockaddr_in * sock;
    
    sock           = (struct sockaddr_in*) _ipaddr.getSockAddr();
    sock->sin_port = htons(port);
    _addrstr       = IpAddr::ntop(ipaddr);
    _hoststr       = _addrstr;
    _hoststr.append(":").append(StringUtils::toString(port));
}

Socket::Socket ( ipv6addr_t ipaddr, uint16_t port, SocketType type, int protocol )
    throw ( SocketException )
    : _ipaddr(ipaddr),
      _socktype(type),
      _proto(protocol),
      _port(port),
      _bound(false),
      _connected(false),
      _block(false),
      _noUdpClose(false)
{
    Socket::ResetDescriptor(this->_fd);

    if ( _socktype <= SOCKTYPE_NONE || _socktype > SOCKTYPE_RAW )
        throw SocketException("Socket error: Invalid Socket type");
    if ( _proto < 0 || _proto > 255 )
        throw SocketException("Socket error: Invalid protocol");
    if ( ipaddr == ipv6addr_any && _socktype < SOCKTYPE_SERVER )
        throw SocketException("Socket error: Invalid IP Address");

    sockaddr_in6 * sock;

    sock            = (struct sockaddr_in6*) _ipaddr.getSockAddr();
    sock->sin6_port = htons(port);
    _addrstr        = IpAddr::ntop(ipaddr);
    _hoststr        = _addrstr;
    _hoststr.append("/:").append(StringUtils::toString(port));
}


Socket::Socket ( sockaddr_t * sa, uint16_t port, SocketType type, int protocol )
    throw ( SocketException )
    : _ipaddr(sa),
      _socktype(type),
      _proto(protocol),
      _port(port),
      _bound(false),
      _connected(false),
      _block(false),
      _noUdpClose(false)
{
    Socket::ResetDescriptor(this->_fd);

    if ( _ipaddr.ipv4() ) {
        sockaddr_in * sock  = (struct sockaddr_in*) _ipaddr.getSockAddr();
        sock->sin_port      = htons(port);
    } else if ( _ipaddr.ipv6() ) {
        sockaddr_in6 * sock = (struct sockaddr_in6*) _ipaddr.getSockAddr();
        sock->sin6_port     = htons(port);
    }

    _addrstr = IpAddr::ntop(_ipaddr.getSockAddr());
    _hoststr = _addrstr;
    _hoststr.append("/:").append(StringUtils::toString(port));
}


Socket::Socket ( addrinfo * ai )
    throw ( SocketException )
    : _socktype(SOCKTYPE_CLIENT),
      _bound(false),
      _connected(false),
      _block(false),
      _noUdpClose(false)
{
    Socket::ResetDescriptor(this->_fd);

    switch ( ai->ai_socktype )
    {
        case SOCK_STREAM:
            _proto = SOCKET_TCP;
            break;
        case SOCK_DGRAM:
            _proto = SOCKET_UDP;
            break;
        case SOCK_RAW:
            _socktype = SOCKTYPE_RAW;
            break;
        default:
            throw SocketException("Unsupported socket protocol");
            break;
    }

    if ( (ai->ai_flags & AI_PASSIVE) )
        _socktype = SOCKTYPE_SERVER;

    _ipaddr  = IpAddr((sockaddr_t*) ai->ai_addr);
 
    if ( _ipaddr.getFamily() == AF_INET ) {
        sockaddr_in * sock  = (sockaddr_in*) _ipaddr.getSockAddr();
        _port    = ntohs(sock->sin_port);
    } else if ( _ipaddr.getFamily() == AF_INET6 ) {
        sockaddr_in6 * sock  = (sockaddr_in6*) _ipaddr.getSockAddr();
        _port    = ntohs(sock->sin6_port);
    }

    _addrstr = _ipaddr.toString();
    _hoststr = _addrstr;
    _hoststr.append("/:").append(StringUtils::toString(_port));
}

Socket::Socket ( sockfd_t & fd, sockaddr_t & csock, SocketType type, int protocol )
    : _fd(fd),
      _ipaddr(&csock),
      _socktype(type),
      _proto(protocol),
      _bound(false),
      _connected(false),
      _block(false),
      _noUdpClose(false)
{
    if ( Socket::IsValidDescriptor(this->_fd) ) 
    {
        if ( _proto == IPPROTO_TCP )
            _connected  = true;
        else if ( _proto == IPPROTO_UDP )
            _noUdpClose = true;
        _bound = true;
    } 
    else 
    {
        _connected = false;
        _bound     = false;
    }
    
    if ( _ipaddr.getFamily() == AF_INET )
    {
        sockaddr_in * sock;
        sock     = (sockaddr_in*) &csock;
        _port    = ntohs(sock->sin_port);
        _addrstr = IpAddr::ntop(sock->sin_addr.s_addr);
        _hoststr = _addrstr;
        _hoststr.append("/:").append(StringUtils::toString(_port));
    }
    else if ( _ipaddr.getFamily() == AF_INET6 )
    {
        sockaddr_in6 * sock;
        sock     = (sockaddr_in6*) &csock;
        _port    = ntohs(sock->sin6_port);
        _addrstr = IpAddr::ntop(sock->sin6_addr);
        _hoststr = _addrstr;
        _hoststr.append("/:").append(StringUtils::toString(_port));
    }
}


Socket::~Socket()
{
    if ( ! this->_noUdpClose )
        this->close();
}

// ----------------------------------------------------------------------

int
Socket::init ( bool block )
{
    if ( ! Socket::IsValidDescriptor(_fd) ) 
    {
        try {
            Socket::InitializeSocket(_fd, _ipaddr, _socktype, _proto);
        } catch ( SocketException & err ) {
            _errstr = err.toString();
            return -1;
        }
    }
      
    if ( _socktype == SOCKTYPE_SERVER ) 
    {
        this->setSocketOption(SocketOption::SetReuseAddr(1));
        
        if ( ! this->bind() )
            return -1;
        if ( _proto == SOCKET_TCP )
            this->listen();
    }
    
    _block = block;

    if ( ! _block )
        Socket::Unblock(this);
    else
        Socket::Block(this);
        
    return 1;
}

// ----------------------------------------------------------------------

int
Socket::bind()
{
    char  serr[ERRORSTRLEN];
    int   r = 0;

    if ( _socktype < SOCKTYPE_SERVER || _bound || ! Socket::IsValidDescriptor(_fd) ) {
            _errstr = "Socket::bind() socket is not initialized";
        return r;
    }
        
    r = ::bind(_fd, (struct sockaddr*) _ipaddr.getSockAddr(), sizeof(sockaddr_t));
    
    if ( r != 0 ) {
        _errstr = "Socket::bind() Failed to bind";
#       ifndef WIN32
        if ( ::strerror_r(errno, serr, ERRORSTRLEN) == 0 )
            _errstr = serr;
#       endif
        return -1;
    }
    
    _bound = true;
    
    return 1;
}

// ----------------------------------------------------------------------

int
Socket::listen()
{
    if ( _socktype != SOCKTYPE_SERVER || _proto != IPPROTO_TCP )
        return 0;
        
    if ( ! this->_bound )
        this->bind();
        
    ::listen(_fd, 1);
    
    _connected = true;
    
    return 1;
}

// ----------------------------------------------------------------------

int
Socket::connect()
{
    if ( _socktype != SOCKTYPE_CLIENT )
        return -1;
        
    if ( _connected )
        return 1;

    if ( ! Socket::IsValidDescriptor(_fd) )
        this->init(_block);

    if ( ::connect(_fd, (struct sockaddr*) _ipaddr.getSockAddr(), sizeof(sockaddr_t)) < 0 )
    {

#       ifdef WIN32
        int  err = WSAGetLastError();

        if ( err == WSAEWOULDBLOCK || err == WSAEALREADY || err == WSAEINVAL ) {
            return 0;
        } else if ( err == WSAEISCONN ) {
            _connected = true;
            return 1;
        }
#       else
        if ( errno == EINPROGRESS )
            return 0;
        else if ( errno == ECONNREFUSED )
            _errstr = "Socket::connect() Connection Refused";
        else 
            _errstr = "Socket::connect() Error in connect attempt";

        char  serr[ERRORSTRLEN];

        if ( ::strerror_r(errno, serr, ERRORSTRLEN) == 0 )
            _errstr = serr;
#     endif

        return -1;
    } else {
        _connected = true;
    }

    return 1;
}

// ----------------------------------------------------------------------

void
Socket::close()
{
   if ( ! this->_noUdpClose ) 
   {
        if ( Socket::IsValidDescriptor(_fd) ) 
        {
           _connected = false;
     
#          ifdef WIN32
           ::closesocket(this->_fd);
#          else
           ::close(this->_fd);
#          endif
       }
       Socket::ResetDescriptor(_fd);
   }

   return;
}

// ----------------------------------------------------------------------

/**  Shutdowns half or the full connection.
  *
  *  @param shut  is the integer value representing which halves of 
  *  the connection to shut. '0' shuts the receive, '1' shuts the send,
  *  and 2 shuts both the send and the receive.
 **/ 
void
Socket::shutdown ( int shut )
{
#   ifdef WIN32
    
    if ( shut == 0 ) {
        ::shutdown(_fd, SD_RECEIVE);
    } else if ( shut == 1 ) {
        ::shutdown(_fd, SD_SEND);
    } else if ( shut == 3 ) {
        ::shutdown(_fd, SD_BOTH);
        _connected = false;
    }
    
#   else
    
    ::shutdown(_fd, shut);
    
#   endif
    
    return;
}

// ----------------------------------------------------------------------

Socket*
Socket::accept()
{
    return this->accept(Socket::factory);
}

/**  The SocketFactory provides a mechanism for instantiating derived Socket 
  *  objects while properly maintaining this interface. A derived implementation 
  *  would also derive the SocketFactory class in order to instantiate the 
  *  derived type prior to casting back to the base Socket* type.
 **/
Socket*
Socket::accept ( SocketFactory & factory )
{
    Socket     * client = NULL;
    sockaddr_t   csock;
    socklen_t    len;
    sockfd_t     cfd;

    if ( _socktype < SOCKTYPE_SERVER )
        return NULL;

    len = sizeof(csock);
    ::memset(&csock, 0, len);

    if ( _proto == SOCKET_TCP )
    {
        if ( (cfd = ::accept(_fd, (struct sockaddr*) &csock, &len)) < 0 )
            return NULL;
        client = factory(cfd, csock, _socktype, _proto);
    }
    else if ( _proto == SOCKET_UDP )
    {
        client = factory(_fd, csock, _socktype, _proto);
    }

    if ( !_block )
        Socket::Unblock(client);

    return client;
}

// ----------------------------------------------------------------------

bool
Socket::isConnected()
{
#   ifdef WIN32
    
    if ( ! Socket::IsValidDescriptor(_fd) )
        _connected = false;

    return _connected;
    
#   else

    if ( !_connected || _proto == IPPROTO_UDP )
        return _connected;

    pollfd  wset;
    char    serr[ERRORSTRLEN];

    wset.fd     = this->getDescriptor();
    wset.events = POLLOUT | POLLERR;

    if ( poll(&wset, 1, 0) < 0 ) 
    {
        if ( errno == EINTR )
            return true;

        if ( ::strerror_r(errno, serr, ERRORSTRLEN) == 0 )
            _errstr = serr;
        
        return false;
    }

    return true;
#   endif
}

// ----------------------------------------------------------------------

ssize_t
Socket::write ( const void * vptr, size_t n )
{
    ssize_t   st  = 0;

    if ( _socktype == SOCKTYPE_RAW || (_proto == SOCKET_UDP && ! _connected) ) {
        st  = ::sendto(_fd, (const char*) vptr, n, 0,
              (struct sockaddr*) _ipaddr.getSockAddr(), sizeof(sockaddr_t));
    } else {
        st  = this->nwriten(vptr, n);
    }

    return st;
}

// ----------------------------------------------------------------------

ssize_t
Socket::readFrom ( void * vptr, size_t n, sockaddr_t & csock )
{
    socklen_t len = 0;
    ssize_t   rd  = 0;

    if ( _proto == SOCKET_TCP && _socktype != SOCKTYPE_RAW )
        return -1;

    len = sizeof(csock);
    rd  = ::recvfrom(_fd, (char*) vptr, n, 0, (struct sockaddr*) &csock, &len);

    if ( rd < 0 ) {
#       ifdef WIN32
        int  err  = WSAGetLastError();
        if ( err == WSAEINTR || err == WSAEWOULDBLOCK )
            rd = 0;
#       else
        if ( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK )
            rd = 0;
#       endif
    }

    return rd;
}

// ----------------------------------------------------------------------

ssize_t
Socket::read ( void * vptr, size_t n )
{
    sockaddr_t csock;
    ssize_t    rd  = 0;

    if ( _proto == SOCKET_UDP && ! _connected ) {
        rd = this->readFrom(vptr, n, csock);
    } else {
        rd = this->nreadn(vptr, n);
    }

    return rd;
}

// ----------------------------------------------------------------------

void
Socket::setBlocking()
{
    Socket::Block(this);
    _block = true;
}

void
Socket::setNonBlocking()
{
    Socket::Unblock(this);
    _block = false;
}

bool
Socket::isBlocking()
{
    return this->_block;
}

// ----------------------------------------------------------------------

SocketOption
Socket::getSocketOption ( SocketOption opt )
{
    SocketOption ropt;
    socklen_t    vlen;
    int          r, v;

    vlen = sizeof(v);

#   ifdef WIN32
    r = ::getsockopt(_fd, opt.level(), opt.id(), (char*)&v, &vlen);
#   else
    r = ::getsockopt(_fd, opt.level(), opt.id(), &v, &vlen);
#   endif

    if ( r == 0 )
        ropt = SocketOption(opt.level(), opt.id(), v, opt.name());

    return ropt;
}

// ----------------------------------------------------------------------

int
Socket::setSocketOption ( int level, int optname, int optval )
{
    socklen_t  len;

    len = (socklen_t) sizeof(int);

    if ( ! Socket::IsValidDescriptor(_fd) )
        return -1;

#   ifdef WIN32
    if ( ::setsockopt(_fd, level, optname, (const char*) &optval, len) < 0 ) {
        _errstr = "Socket: Error in call to setsockopt()";
        return -1;
    }
#   else
    char  serr[ERRORSTRLEN];
    if ( ::setsockopt(_fd, level, optname, (const void*) &optval, len) < 0 ) {
        // test for EOPNOTSUPP here
        if ( ::strerror_r(errno, serr, ERRORSTRLEN) == 0 )
            _errstr = serr;
        return -1;
    }
#   endif

    return 1;
}

int 
Socket::setSocketOption ( SocketOption opt )
{
    return this->setSocketOption(opt.level(),
                                 opt.id(),
                                 opt.value());
}

// ----------------------------------------------------------------------

const sockfd_t&
Socket::getDescriptor() const
{
    return _fd;
}

// ----------------------------------------------------------------------

const SocketType&
Socket::getSocketType() const
{
    return this->_socktype;
}

// ----------------------------------------------------------------------

const uint16_t&
Socket::getPort() const
{
    return this->_port;
}

// ----------------------------------------------------------------------

const int&
Socket::getSocketProtocol() const
{
    return this->_proto;
}

// ----------------------------------------------------------------------

const std::string&
Socket::getAddressString() const
{
    return this->_addrstr;
}


const std::string&
Socket::getHostString() const
{
    return this->_hoststr;
}

// ----------------------------------------------------------------------

ipv4addr_t
Socket::getAddress() const
{
    return _ipaddr.getAddr4();
}

const sockaddr_t*
Socket::getSockAddr() const
{
    return _ipaddr.getSockAddr();
}

// ----------------------------------------------------------------------
/**  THese set/get methods are for avoiding calling close on a UDP socket
 *   descriptor. This only gets set true by the protected constructor
 *   when creating a UDP construct of a 'server-client' object. Calling
 *   close on a server-client fd in UDP would result in closing on our
 *   server object as well. This is a special-case condition only needed
 *   when utilizing the SocketFactory with UDP sockets.
 */
void
Socket::setUdpNoClose ( bool noclose )
{
    this->_noUdpClose = noclose;
}

bool
Socket::getUdpNoClose() const
{
    return this->_noUdpClose;
}

// ----------------------------------------------------------------------

const std::string&
Socket::getErrorString() const
{
    return _errstr;
}

// ----------------------------------------------------------------------

/** Static function to set a Socket to non-blocking */
void
Socket::Unblock ( Socket * s )
{
    if ( s == NULL )
        return;

#   ifdef WIN32
    
    unsigned long  on = 1;
    ioctlsocket(s->getFD(), FIONBIO, &on);
    
#   else
    
    int flags = ::fcntl(s->getDescriptor(), F_GETFL, 0);
    ::fcntl(s->getDescriptor(), F_SETFL, flags | O_NONBLOCK);

#   endif

    return;
}

/** Static function to set a Socket to blocking */
void
Socket::Block ( Socket * s ) 
{
    if ( s == NULL )
        return;

#   ifdef WIN32
    
    unsigned long  on = 0;
    ioctlsocket(s->getFD(), FIONBIO, &on);
    
#   else 
    
    int  flags  = ::fcntl(s->getDescriptor(), F_GETFL, 0);
    ::fcntl(s->getDescriptor(), F_SETFD, flags & ~O_NONBLOCK);

#   endif

    return;
}

// ----------------------------------------------------------------------

/**  A static function for ensuring a file descriptor is valid. This
  *  provides a consistent method for testing descriptors across different
  *  platforms.
 */
bool
Socket::IsValidDescriptor ( const sockfd_t & fd )
{
#   ifdef WIN32
    if ( fd <= 0 ) 
        return false;
#   else
    if ( fd < 0 ) 
        return false;
#   endif
    return true;
}
    
void
Socket::ResetDescriptor ( sockfd_t & fd )
{
#   ifdef WIN32
    fd = 0;
#   else
    fd = -1;
#   endif
}

// ----------------------------------------------------------------------

/**  Internal Socket method for performing a non-blocking write, if applicable. */
ssize_t
Socket::nwriten ( const void * vptr, size_t n )
{
    size_t      nleft;
    ssize_t     nwritten;
    const char* ptr = (const char*) vptr;

    nleft = n;

    while ( nleft > 0 ) 
    {
        if ( (nwritten = ::send(_fd, ptr, nleft, 0)) <= 0 ) 
        {
#           ifdef WIN32

            int  err  = WSAGetLastError();
            if ( err == WSAEINTR )
                nwritten = 0;
            else if ( err == WSAEWOULDBLOCK )
                return(n-nleft);
            else
                return -1;

#           else

            if ( errno == EINTR )
                nwritten = 0;
            else if ( errno == EWOULDBLOCK || errno == EAGAIN || errno == EINPROGRESS )
                return(n-nleft);
            else
                return -1;

#           endif
        }
        nleft -= nwritten;
        ptr   += nwritten;
    }

    return n;
}

// ----------------------------------------------------------------------

/**  Internal Socket method for performing a non-blocking read, if applicable. */
ssize_t
Socket::nreadn ( void * vptr, size_t n )
{
    size_t nleft;
    int    nread;
    char*  ptr;
 
    ptr   = (char*) vptr;
    nleft = n;

    while ( nleft > 0 ) 
    {
        if ( (nread = ::recv(_fd, ptr, nleft, 0)) < 0 ) 
        {
#           ifdef WIN32
                
            int  err  = WSAGetLastError();
            if ( err == WSAEINTR ) 
                nread = 0;
            else if ( err == WSAEWOULDBLOCK )
                return(n-nleft);
            else
                return -1;
                
#           else
            
            if ( errno == EINTR )
                nread = 0;
            else if ( errno == EWOULDBLOCK || errno == EAGAIN || errno == EINPROGRESS )  
                return(n-nleft);
            else
                return -1;
            
#           endif
        } 
        else if ( nread == 0 ) 
        {
            return -1;
        }
        nleft -= nread;
        ptr   += nread;
    }

    return(n-nleft);
    
}

// ----------------------------------------------------------------------

/** Static function for initializing a socket descriptor */
void
Socket::InitializeSocket ( sockfd_t & fd, IpAddr & addr, int socktype, int proto )
    throw ( SocketException )
{
    std::string errstr = "Socket::initSocket() Fatal Error ";

    if ( socktype == SOCKTYPE_RAW ) 
    {
        fd = ::socket(addr.getFamily(), SOCK_RAW, proto);
    }
    else
    {
        if ( proto == SOCKET_TCP ) {
            fd = ::socket(addr.getFamily(), SOCK_STREAM, 0);
        } else if ( proto == SOCKET_UDP ) {
            fd = ::socket(addr.getFamily(), SOCK_DGRAM, 0);
        } else {
            errstr.append(": Unsupported protocol");
            throw SocketException(errstr);
        }
    }

    if ( ! Socket::IsValidDescriptor(fd) ) 
    {
#       ifdef WIN32
        errstr.append(": Failed to initialize socket");
#       else
        char   serr[ERRORSTRLEN];

        if ( errno == EACCES || errno == EPERM ) {
            errstr.append("EACCES: Permission denied");
        } else if ( errno == EAFNOSUPPORT ) {
            errstr.append("EAFNOSUPPORT: Address Family not supported");
        } else if ( errno == EINVAL ) {
            errstr.append("EINVAL: Unknown protocol or PF not supported");
        } else {
            char const * str = ::strerror_r(errno, serr, ERRORSTRLEN);
            errstr.append(str);
        }
#       endif

        throw SocketException(errstr);
    }

    return;
}

// ----------------------------------------------------------------------

uint16_t
Socket::IpChkSum ( uint16_t * t, int n )
{
    uint16_t res = 0;
    uint32_t sum = 0;
    uint8_t  ob  = 0;

    while ( n > 1 ) {
        sum += *t++;
        n   -= 2;
    }

    if ( n == 1 ) {
        *(uint8_t*)(&ob) = *(uint8_t*) t;
        sum += ob;
    }

    sum  = ( sum >> 16 ) + ( sum & 0xFFFF );
    sum += ( sum >> 16 );
    res  = ~sum;

    return res;
}

// ----------------------------------------------------------------------

} // namespace

//  _TCANETPP_SOCKET_CPP_ 

