/**
  * @file Addrinfo.h
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
#ifndef _TCANETPP_ADDRINFO_H_
#define _TCANETPP_ADDRINFO_H_

#include <string>
#include "tcanetpp_ip.h"



namespace tcanetpp {


class AddrInfo {

  public:

    /* AddrInfo factory used by static methods */
    class AddrInfoFactory {
      public:
        virtual ~AddrInfoFactory() {}
        virtual AddrInfo* operator() ( struct addrinfo * ai );
    };

    static AddrInfoFactory  factory;


  public:

    AddrInfo();

    AddrInfo ( const std::string & name, 
               const std::string & service = "" );


    virtual ~AddrInfo();


  protected:

    AddrInfo ( struct addrinfo   * ai );


  public:

    bool        isValid() const;


    addrinfo*   begin();
    addrinfo*   next();
    addrinfo*   get();

    sockaddr_t* getAddr();
    sockaddr_t* getSockAddr() { return this->getAddr(); }


    int         getFlags() const;
    bool        setFlags ( int flags );

    int         getFamily() const;
    bool        setFamily ( int family );

    int         getSocktype() const;
    bool        setSocktype ( int type );

    int         getProtocol() const;
    bool        setProtocol ( int proto );

    size_t      getAddrLen() const;
    size_t      getAddrlen() const { return this->getAddrLen(); }
    char*       getCanonName() const;

    int         getError() const;


  public:

    /* static factory methods */
    static AddrInfo*   GetAddrInfo ( const std::string & host,
                                     const std::string & svc = "" );

    static AddrInfo*   GetAddrInfo ( const std::string & host,
                                     uint16_t            port );


    /* static methods for getaddrinfo/getnameinfo */
    static int         GetAddrInfo ( const std::string & host,
                                     const addrinfo    * hints,
                                     addrinfo         ** res );

    static int         GetAddrInfo ( const std::string & host,
                                     const std::string & svc,
                                     const addrinfo    * hints,
                                     addrinfo         ** res );

    static int         GetNameInfo ( const ipv4addr_t  & addr,
                                     std::string       & result,
                                     int                 flags);

    static int         GetNameInfo ( const ipv6addr_t  & addr,
                                     std::string       & result,
                                     int                 flags );

    static int         GetNameInfo ( const sockaddr    * sock,
                                     socklen_t           salen,
                                     std::string       & result,
                                     int                 flags );

    /* methods for legacy support */
    static std::string GetHostName();
    static std::string GetHostName ( const ipv4addr_t  & addr );
    static ipv4addr_t  GetHostAddr ( const std::string & host );

    /* factory methods for generating addrinfo hints */
    static addrinfo    GetTCPServerHints();
    static addrinfo    GetUDPServerHints();
    static addrinfo    GetTCPClientHints();
    static addrinfo    GetUDPClientHints();
    static int       ai_error;

  protected:

    struct addrinfo*  _ai;
    struct addrinfo*  _nxt;
    int               _err;

};

} // namespace

#endif // _TCANETPP_ADDRINFO_H_

