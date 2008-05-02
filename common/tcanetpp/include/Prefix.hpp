/**
  *  Prefix - Represents a network CIDR address or a network IP Address 
  *     in the standard CIDR notation of x.x.x.x/yy.
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.1
 **/
#ifndef _TCANETPP_PREFIX_HPP_
#define _TCANETPP_PREFIX_HPP_

#include <vector>

#include "tcanetpp_types.h"


namespace tcanetpp {


class Prefix {

  public:

    Prefix() {}

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
        return(_pfx.addr < prefix._pfx.addr);
    }

    inline ipv4addr_t    getPrefix() const
    { 
        return _pfx.addr;
    }

    inline uint8_t       getPrefixLen() const
    {
        uint8_t masklen = *((uint8_t*)&_pfx.addrlen);
        return( *(uint8_t*)masklen );
    }

    inline const cidr_t& getCidr() const
    { 
        return _pfx;
    }
    

  protected:

    cidr_t	_pfx;

};


typedef std::vector<Prefix>  PrefixList;


}  // namespace


#endif  //  _TCANETPP_PREFIX_HPP_

