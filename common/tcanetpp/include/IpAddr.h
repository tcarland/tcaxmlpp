/**
  * @file IpAddr.h
  *
  * These are defined within this library to avoid having to
  * use platform specific defines for various headers. This also
  * means we avoid the issue of some fields varying slightly in
  * convention across platforms.
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
#ifndef _TCANETPP_IPADDR_H_
#define _TCANETPP_IPADDR_H_

#include <string>
#include <set>
#include <vector>

#include "tcanetpp_ip.h"


namespace tcanetpp {

/*
 *   ::0:0/96       ipv4 in ipv6
 *   ::ffff:0:0/96  ipv4 mapped ipv6
 *
 *  0     16    32    48    64    80    96   112   128
 *  | 0000:0000 : 0000:0000 : 0000:0000 : 0000:0000 |
 *  |           |           |           |           |
 */

class IpAddr;


typedef std::set<IpAddr>     IpAddrSet;
typedef std::vector<IpAddr>  IpAddrList;


class IpAddr {

  public:

    IpAddr();
    IpAddr ( const sockaddr * sa );
    IpAddr ( const ipv4addr_t & addr, uint8_t mb = 32 );
    IpAddr ( const ipv6addr_t & addr, uint8_t mb = 64 );
    IpAddr ( const IpAddr & ipaddr );

    virtual ~IpAddr();


    bool                operator==   ( const IpAddr & ipaddr ) const;
    bool                operator<    ( const IpAddr & ipaddr ) const;

    bool                ipv4() const;
    bool                ipv6() const;

    ipv6addr_t          getAddr() const;
    ipv6addr_t          getAddr6() const;
    ipv6addr_t          getPrefix6() const;
    uint8_t             getPrefix6Len() const { return this->getPrefixLen(); }

    ipv4addr_t          getAddr4() const;
    ipv4addr_t          getPrefix() const;
    uint8_t             getPrefixLen() const;
    cidr_t              getCidr() const;

    sockaddr_t*         getSockAddr();
    const sockaddr_t*   getSockAddr() const;

    std::string         toString() const;
    bool                isLoopback() const;


  public:   /*  static functions */

    static bool         IsLoopback   ( const ipv4addr_t  & addr );
    static bool         IsLoopback   ( const ipv6addr_t  & addr );

    static std::string  ntop         ( const ipv4addr_t  & addr );
    static std::string  ntop         ( const ipv6addr_t  & addr );
    static std::string  ntop         ( const sockaddr_t  * sock );

    static int          pton         ( const std::string & ipstr,
                                       ipv4addr_t        & addr );
    static int          pton         ( const std::string & ipstr,
                                       ipv6addr_t        & addr );

    static std::string  ether_ntop   ( const ethaddr_t   * ethaddr );

    static bool         IsBasePrefix ( const ipv4addr_t  & pfx, uint8_t mb );
    static ipv4addr_t   ToBasePrefix ( const ipv4addr_t  & pfx, uint8_t mb );
    static std::string  ToPrefixStr  ( const IpAddr      & pfx );
    static int          ToIpAddr     ( const std::string & str,
                                       IpAddr            & ipaddr );

    static ipv4addr_t   BitsToMask   ( uint8_t mb );
    static uint8_t      SubnetValue  ( const ipv4addr_t  & addr, uint8_t pos );

    static bool         MatchPrefix  ( const IpAddr      & pfx,
                                       const ipv4addr_t  & addr );

    static uint32_t     RandomValue  ( double     range );
    static IpAddr       RandomAddr   ( IpAddr   & agg );
    static ipv4addr_t   RandomPrefix ( ipv4addr_t addr, uint8_t mb );

    static bool         DeAggregate  ( const IpAddr & pfx, uint8_t mb,
                                       IpAddrList   & v );

    static int          GetCidrRange ( uint8_t mb, uint8_t * subnet = NULL );
    static int          GetIpv6Range ( uint8_t mb );


  private:

    sockaddr_t          _saddr;
    uint8_t             _mb;

};

} // namespace


#endif // _TCANETPP_IPADDR_H_

