/**
  *  Prefix - Represents a network CIDR address or a network IP Address 
  *     in the standard CIDR notation of x.x.x.x/yy.
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
#ifndef _TCANETPP_PREFIX_HPP_
#define _TCANETPP_PREFIX_HPP_

#include <vector>

#include "tcanetpp_types.h"


namespace tcanetpp {

class Prefix;
typedef std::vector<Prefix>  PrefixList;


class Prefix {

  public:

    Prefix()
    {
        _pfx.addr     = 0;
        _pfx.addrlen  = 0;
    }

    Prefix ( ipv4addr_t addr, uint8_t len )
    {
        _pfx.addr     = addr;
        _pfx.addrlen  = len;
    }

    Prefix ( const cidr_t & cidr ) 
        : _pfx(cidr) 
    {}
      
    Prefix ( const Prefix & p ) 
    {
	_pfx.addr    = p.getPrefix();
	_pfx.addrlen = p.getPrefixLen();
    }

    virtual ~Prefix() {}


    bool   operator==  ( const Prefix & prefix ) const
    {
        return( (_pfx.addr == prefix._pfx.addr) 
                && (_pfx.addrlen == prefix._pfx.addrlen) );
    }
    
    bool   operator<   ( const Prefix & prefix ) const
    {
        if ( _pfx.addr == prefix._pfx.addr )
            return(_pfx.addrlen < prefix._pfx.addrlen);
        return(_pfx.addr < prefix._pfx.addr);
    }

    inline ipv4addr_t    getPrefix() const
    { 
        return _pfx.addr;
    }

    inline uint8_t       getPrefixLen() const
    {
        uint8_t masklen = *((uint8_t*)&_pfx.addrlen);
        return masklen;
    }

    inline const cidr_t& getCidr() const
    { 
        return _pfx;
    }
    

  protected:

    cidr_t	_pfx;

};


}  // namespace

#endif  //  _TCANETPP_PREFIX_HPP_

