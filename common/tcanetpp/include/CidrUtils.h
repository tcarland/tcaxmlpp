/** 
  * @file CidrUtils.h
  * 
  *   The CidrUtils class is a collection of static utility functions for 
  * manipulating IP Address, Prefix, and Cidr's.
  *  
  * Copyright (c) 2008-2011 Timothy Charlton Arland 
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
#ifndef _TCANETPP_CIDRUTILS_H_
#define _TCANETPP_CIDRUTILS_H_

#include <string>
#include <vector>

#include "tcanetpp_ip.h"
#include "Prefix.hpp"
#include "IpAddr.h"


namespace tcanetpp {


/**  The CidrUtils class is a collection of static utility functions for 
  * manipulating IP Address, Prefix, and Cidr's.
 **/
class CidrUtils {
	
public:


    static std::string  ToString        ( const Prefix      & pfx );
    static std::string  ToString        ( const ipv4addr_t  & addr, uint8_t mb );

    static int          StringToCidr    ( const std::string & cidrStr,
                                          Prefix            & pfx );


    static int          GetCidrRange    ( uint8_t mb, uint8_t * subnet_pos );
    static int          GetCidrRange    ( uint8_t mb );

    static inline int   GetBlockSize    ( uint8_t mb, uint8_t * subnet_pos )
    {
        return CidrUtils::GetCidrRange(mb, subnet_pos);
    }

    static uint8_t      SubnetValue     ( ipv4addr_t addr, uint8_t subnet_pos );


};

} // namespace


#endif  // _TCANETPP_CIDRUTILS_H_

