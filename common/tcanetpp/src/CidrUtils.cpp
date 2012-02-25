/**  
  * @file CidrUtils.cpp
  * 
  *   The CidrUtils class is a collection of static utility functions for 
  * manipulating IP Address, Prefix, and Cidr's.
  *  
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @Author  tca@charltontechnology.net
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
#define _TCANETPP_CIDRUTILS_CPP_

#include <math.h>

#ifndef WIN32
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netinet/ether.h>
# include <netdb.h>
#endif

#include "CidrUtils.h"
#include "Socket.h"
#include "StringUtils.h"


namespace tcanetpp {


//-------------------------------------------------------------------//


/**@{
  *   Converts the given Prefix object to a cidr formatted string. 
 **/
std::string
CidrUtils::ToString ( const Prefix & pfx )
{
    return CidrUtils::ToString(pfx.getPrefix(), pfx.getPrefixLen());
}


std::string
CidrUtils::ToString ( const ipv4addr_t & addr, uint8_t mb )
{
    char  cidr[INET4_CIDRSTRLEN];

    std::string  cidrStr = IpAddr::ntop(addr);

#   ifdef WIN32
    ::_snprintf(cidr, INET4_CIDRSTRLEN, "%s%s%u", cidrStr.c_str(), "/", mb);
#   else
    ::snprintf(cidr, INET4_CIDRSTRLEN, "%s%s%u", cidrStr.c_str(), "/", mb);
#   endif

    cidrStr.assign(cidr);

    return cidrStr;
}
/*@}*/

//-------------------------------------------------------------------//

int
CidrUtils::StringToCidr ( const std::string & cidrStr, Prefix & pfx )
{
    Prefix      prefix;
    std::string addrstr = "";
    ipv4addr_t  addr    = 0;
    uint16_t    mb      = 0;  // using a short intentionally
    
    std::string::size_type  indx;

    indx = cidrStr.find_first_of('/');
    if ( indx == std::string::npos ) {
        addrstr = cidrStr;
        mb = 32;
    } else {
        addrstr = cidrStr.substr(0, indx);
    }

    if ( IpAddr::pton(addrstr, addr) <= 0 )
        return 0;
  
    if ( mb == 0 ) {
        addrstr = cidrStr.substr(indx+1);
        // must use a short here, uint8_t is an unsigned char 
        // and is interpreted as char by fromString()
        mb = (uint16_t) StringUtils::fromString<uint16_t>(addrstr);
    }

    if ( mb < 1 || mb > 32 )
	mb = 32;

    pfx = Prefix(addr, mb);

    return 1;
}


//-------------------------------------------------------------------//

int
CidrUtils::GetCidrRange ( uint8_t mb, uint8_t * subnet_pos )
{
    uint8_t  pos = 1;

    while ( mb >= 8 ) {
        mb -= 8;
        pos++;
    }

    if ( subnet_pos )
        *subnet_pos = pos;

    return( ::pow((double)2, (double)(8 - mb)) );
}

int
CidrUtils::GetCidrRange ( uint8_t mb )
{
    return CidrUtils::GetCidrRange(mb, NULL);
}

//-------------------------------------------------------------------//


} //namespace

//  _TCANETPP_CIDRUTILS_CPP_ 
