/**
  * @file Prefix.hpp
  *
  *   Represents a network CIDR address or a network IP Address 
  * in the standard CIDR notation of x.x.x.x/yy.
  *  
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author tcarland@gmail.com
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
#ifndef _TCANETPP_PREFIX_HPP_
#define _TCANETPP_PREFIX_HPP_

#include <vector>

#include "tcanetpp_types.h"


namespace tcanetpp {

class Prefix;
typedef std::vector<Prefix>  PrefixList;


/**  An instance of the Prefix class represents a single 'cidr' 
  *  format IP Address and its associated mask length. The class 
  *  is used with the tcanetpp library and provides convenient 
  *  sorting and equality operators for use with the Standard 
  *  Template Library. This legacy class supports IPv4 only,
  *  use the IpAddr class for both IPv4 and IPv6 support.
 **/
class Prefix {

  public:

    Prefix()
    {
        _pfx.addr = 0;
        _pfx.mb   = 0;
    }

    Prefix ( ipv4addr_t addr, uint16_t mb )
    {
        _pfx.addr = addr;
        _pfx.mb   = mb;
    }

    Prefix ( const cidr_t & cidr ) 
    {
        _pfx.addr = cidr.addr;
        _pfx.mb   = cidr.mb;
    }

    Prefix ( const Prefix & p ) 
    {
        _pfx.addr = p.getPrefix();
        _pfx.mb   = p.getPrefixLen();
    }

    virtual ~Prefix() {}


    inline bool operator== ( const Prefix & prefix ) const
    {
        return( (_pfx.addr == prefix._pfx.addr) 
               && (_pfx.mb == prefix._pfx.mb) );
    }
    
    inline bool operator<  ( const Prefix & prefix ) const
    {
        if ( _pfx.addr == prefix._pfx.addr )
            return(_pfx.mb < prefix._pfx.mb  );
        return(_pfx.addr < prefix._pfx.addr);
    }

    inline ipv4addr_t  getPrefix() const
    { 
        return _pfx.addr;
    }

    inline uint8_t  getPrefixLen() const
    {
        uint8_t mb = *((uint8_t*)&_pfx.mb  );
        return mb;
    }

    inline const cidr_t& getCidr() const
    { 
        return _pfx;
    }
    

  protected:

    cidr_t      _pfx;

};


}  // namespace

#endif  //  _TCANETPP_PREFIX_HPP_

