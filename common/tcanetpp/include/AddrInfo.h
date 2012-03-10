/**
  * @file Addrinfo.h
  *
  * For handling struct addrinfo and getaddrinfo/getnameinfo
  *
  * Copyright (c) 2012 Timothy Charlton Arland
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

#include "IpAddr.h"
#include "tcanetpp_ip.h"



namespace tcanetpp {


class AddrInfo {

  public:

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
    size_t      getAddrLen() const;
    size_t      getAddrlen() const { return this->getAddrLen(); }

    bool        setFlags    ( int flags );
    int         getFlags()  const;

    bool        setFamily   ( int family );
    int         getFamily() const;

    bool        setSocktype ( int type );
    int         getSocktype() const;

    int         getProtocol() const;
    bool        setProtocol ( int proto );

    char*       getCanonName() const;

    int         getError() const;


  public:

    /* static factory methods */

    static AddrInfo*   GetAddrInfo ( const std::string & host,
                                     const std::string & svc = "" );

    static AddrInfo*   GetAddrInfo ( const std::string & host,
                                     uint16_t            port );


    /* static utility methods for getaddrinfo/getnameinfo */

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

    /* static utility methods for legacy support */

    static std::string GetHostName();
    static std::string GetHostName ( const ipv4addr_t  & addr );
    static ipv4addr_t  GetHostAddr ( const std::string & host );
    static int         GetAddrList ( const std::string & host,
                                     IpAddrList        & v );

    /* factory methods for generating hints */

    static addrinfo    GetTCPServerHints();
    static addrinfo    GetUDPServerHints();
    static addrinfo    GetTCPClientHints();
    static addrinfo    GetUDPClientHints();

    static std::string GetErrorStr();


  public:

    static std::string ai_error;


  protected:

    struct addrinfo*  _ai;
    struct addrinfo*  _nxt;
    int               _err;

};

} // namespace

#endif // _TCANETPP_ADDRINFO_H_

