/**  
  *  The CidrUtils class is a collection of static utility functions for 
  *  manipulating IP Address, Prefix, and Cidr's.
  *  
  * Copyright (c) 2008, 2009 Timothy Charlton Arland 
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
#ifndef _TCANETPP_CIDRUTILS_H_
#define _TCANETPP_CIDRUTILS_H_

#include <string>
#include <vector>

#include "tcanetpp_types.h"
#include "Prefix.hpp"


namespace tcanetpp {


#define IPV4ADDR_LOOPBACK 16777343

typedef std::vector<ipv4addr_t>   IpAddrList;


class CidrUtils {
	
public:

    static bool         IsBasePrefix    ( ipv4addr_t  addr, uint8_t mb );
    static ipv4addr_t   ToBasePrefix    ( ipv4addr_t  addr, uint8_t mb );

    static std::string  ToString        ( ipv4addr_t     addr );
    static std::string  ToString        ( const Prefix & pfx );
    static std::string  ToString        ( ipv4addr_t     addr, uint8_t mb );

    static int          StringToAddr    ( const std::string & addrStr, 
                                          ipv4addr_t        & addr );
    static int          StringToCidr    ( const std::string & cidrStr,
                                          Prefix            & pfx );

    static ipv4addr_t   BitsToMask      ( uint8_t     mb );
    static uint8_t      MaskToBits      ( ipv4addr_t  addr );

    static int          GetCidrRange    ( uint8_t mb, uint8_t * subnet_pos );
    static int          GetCidrRange    ( uint8_t mb );

    static inline int   GetBlockSize    ( uint8_t mb, uint8_t * subnet_pos )
    {
        return CidrUtils::GetCidrRange(mb, subnet_pos);
    }

    static uint8_t      SubnetValue     ( ipv4addr_t addr, uint8_t subnet_pos );


    static bool         DeAggregate     ( Prefix     & p, uint8_t masklen, 
                                          PrefixList & v );


    static bool         MatchCidr       ( Prefix     & p, ipv4addr_t addr );


    static std::string  ntop            ( ipv4addr_t          addr );
    static int          pton            ( const std::string & ipstr, 
                                          ipv4addr_t        & addr );


    static std::string  GetHostName();

    static std::string  GetHostName     ( ipv4addr_t          addr );

    static ipv4addr_t   GetHostAddr();
    static ipv4addr_t   GetHostAddr     ( const std::string & host );
    static void         GetHostAddrList ( const std::string & host, 
                                          IpAddrList        & addrlist );

    static bool         IsLoopback      ( ipv4addr_t addr );

};

} // namespace


#endif  // _TCANETPP_CIDRUTILS_H_

