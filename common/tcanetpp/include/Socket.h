/**
  *  Socket.h -  The base Socket class providing an object-oriented
  *   interface to sockets for both Unix and Win32 platforms.
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
#ifndef _TCANETPP_SOCKET_H_
#define _TCANETPP_SOCKET_H_


#ifdef WIN32
# include <Winsock2.h>
# include <ws2tcpip.h>
# pragma warning (disable:4267)
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
#endif

#include <string.h>
#include <string>
#include <cstdlib>


#include "tcanetpp_types.h"
#include "Exception.hpp"
#include "SocketOption.h"


namespace tcanetpp {



#define SOCKET_TCP  IPPROTO_TCP
#define SOCKET_UDP  IPPROTO_UDP
#define SOCKET_ICMP IPPROTO_ICMP


/**  Socket types for Socket constructor  */
typedef enum SocketType
{
    SOCKTYPE_NONE = 0,
    SOCKTYPE_CLIENT = 1,
    SOCKTYPE_SERVER_CLIENT = 2,
    SOCKTYPE_SERVER = 3,
    SOCKTYPE_RAW = 4
} sockType_t;



/**  SocketException class for fatal Socket class errors */
class SocketException : public Exception {
public:
    SocketException ( const std::string & errstr ) 
        : Exception(errstr) {}
    virtual ~SocketException() throw() {}
};


/**  The Socket class represents a BSD style socket using either
  *  the GNU C Library or Winsock2 in the case of WIN32 hosts and 
  *  provides an object-oriented interface for performing socket
  *  io.
 **/
class Socket {
  public:

    class SocketFactory {
      public:
        virtual ~SocketFactory() {}
            
        virtual Socket* operator() ( sockfd_t & fd, sockaddr_in & csock, 
                                     SocketType type, int protocol );
    };


    class UdpSocketFactory : public SocketFactory {
        sockaddr_in  _csock;
      public:
        explicit UdpSocketFactory ( sockaddr_in sock )
            : _csock(sock)
        {}
        virtual ~UdpSocketFactory() {}

        virtual Socket* operator() ( sockfd_t & fd, sockaddr_in & csock,
                                     SocketType type, int protocol );
    };

    static SocketFactory  factory;
	

  public:
	
    Socket();

    Socket ( ipv4addr_t ipaddr, uint16_t port, SocketType type, int protocol )
        throw ( SocketException );
    
    virtual ~Socket();
	
	
  protected:
	
    Socket ( sockfd_t & fd, sockaddr_in & csock, SocketType type, int protocol );
	
  public:
    
    virtual int         init ( bool block = false );
    
    int                 bind();
    int                 listen();
    
    virtual int         connect();
    virtual void        close();
    virtual void        shutdown ( int shut );
    
    Socket*             accept();
    Socket*             accept ( SocketFactory & factory );
    
    virtual bool        isConnected();
    bool                isServerSocket() const;
    
    ipv4addr_t          getAddress() const;
    const std::string&  getAddressString() const;
    const std::string&  getHostString() const;

    ipv4addr_t          getAddr() const       { return this->getAddress(); }
    const std::string&  getAddrString() const { return this->getAddressString(); }
    const std::string&  getAddrStr() const    { return this->getAddressString(); }
    const std::string&  getHostStr() const    { return this->getHostString(); }
    
    const sockfd_t&     getDescriptor() const;
    const sockfd_t&     getFD() const    { return this->getDescriptor(); }
    
    const SocketType&   getSocketType() const;
    const int&          getSocketProtocol() const;
    const uint16_t&     getPort() const;

    void                setUdpNoClose ( bool noclose );
    bool                getUdpNoClose() const;

    int                 setSocketOption ( int level, int optname, int optval );
    int                 setSocketOption ( SocketOption sockopt );
    SocketOption        getSocketOption ( SocketOption sockopt );
    
    void                setBlocking();
    void                setNonBlocking();
    bool                isBlocking();
    
    const std::string&  getErrorString() const;
    const std::string&  errorStr() const  { return this->getErrorString(); }
    
    virtual ssize_t     read     ( void * vptr, size_t n );
    virtual ssize_t     write    ( const void * vptr, size_t n );

    virtual ssize_t     readFrom ( void * vptr, size_t n, sockaddr_in & csock );
            
    
  public:	
    
    static void         Block    ( Socket * s );
    static void         Unblock  ( Socket * s );
    
    static ipv4addr_t   pton     ( const std::string & ipstr );
    static std::string  ntop     ( ipv4addr_t addr );
    
    static bool         IsValidDescriptor ( const sockfd_t & fd );
    static void         ResetDescriptor   ( sockfd_t & fd );

    static uint16_t     IpChkSum ( uint16_t * t, int n );
   

  protected:
    
    ssize_t             nwriten  ( const void * vptr, size_t n );
    ssize_t             nreadn   ( void * vptr, size_t n );
    
    
    static void InitializeSocket ( sockfd_t & fd, int type, int proto )
        throw ( SocketException );
    
    
  protected:
    
    std::string	            _addrstr;
    std::string	            _hoststr;
    std::string	            _errstr;
    
    
  private:
    
    sockfd_t                _fd;
    sockaddr_in             _sock;
    SocketType              _socktype;
    int                     _proto;
    uint16_t                _port;
    
    bool                    _bound;
    bool                    _connected;
    bool                    _block;
    bool                    _noUdpClose;
    
};


} // namespace

#endif //  _TCANETPP_SOCKET_H_

