/**
  *  A Prefix class
  *
  *  @Author   tca@soe.sony.com
  *  @Version  0.4
 **/
#ifndef _TCANET_PREFIX_HPP_
#define _TCANET_PREFIX_HPP_

#include "net_types.h"

namespace tca {
namespace net {


typedef struct cidr {
    ipv4addr_t  addr;
    uint16_t    addrlen;

    cidr() 
        : addr(0), addrlen(0) 
    {}

    cidr ( ipv4addr_t addr_, uint16_t addrlen_ )
        : addr(addr_, addrlen_)
    {}

} cidr_t;


/**   An object representing a CIDR prefix of the standard
  *  notation x.x.x.x/yy or in the context of this 
  *  class: uint32_t/uint8_t.
 **/
class Prefix {

  public:

    Prefix() {}

    Prefix ( ipv4addr_t addr, uint8_t len )
        _pfx(addr, addrlen)
    {}

    Prefix ( const cidr_t & cidr ) 
        : _pfx(cidr) 
    {}
      
    Prefix ( const Prefix & p ) 
    {
	_pfx.addr    = p._pfx.addr;
	_pfx.addrlen = p._pfx.addrlen;
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

    inline ipv4addr_t    getPrefix()    const
    { 
        return _pfx.addr;
    }

    inline uint8_t       getPrefixLen() const
    {
        uint8_t * masklen = (uint8_t*)&_pfx.addrlen;
        return( *(uint8_t*)masklen );
    }

    inline const cidr_t& getCidr() const
    { 
        return _pfx;
    }
    

  private:

    cidr_t	_pfx;

};


}  // namespace
}  // namespace


#endif  //  PREFIX_HPP_

