/**  
  *  The CidrUtils class is a collection of static utility functions for 
  *  manipulating IP Address, Prefix, and Cidr's.
  *
  *  @Author  tcarland@gmail.com
  *  @Versoin 0.1
 **/
#ifndef _TCANETPP_CIDRUTILS_H_
#define _TCANETPP_CIDRUTILS_H_

#include <string>
#include <vector>

#include "tcanetpp_types.h"
#include "Prefix.hpp"



namespace tcanetpp {


#define IPV4ADDR_LOOPBACK 16777343

typedef std::vector<ipv4addr_t>  AddrList;


class CidrUtils {
	
public:

    static bool         isBasePrefix    ( ipv4addr_t addr, uint8_t mb );
    static ipv4addr_t   toBasePrefix    ( ipv4addr_t addr, uint8_t mb );

    static std::string  toString        ( ipv4addr_t     addr );
    static std::string  toString        ( const Prefix & pfx );
    static std::string  toString        ( ipv4addr_t     addr, uint8_t mb );

    static int          StringToAddr    ( const std::string & addrStr, 
                                          ipv4addr_t        & addr );
    static int          StringToCidr    ( const std::string & cidrStr,
                                          Prefix            & pfx );

    static ipv4addr_t   bitsToMask      ( uint8_t mb );
    static uint8_t      maskToBits      ( ipv4addr_t addr );

    static int          getCidrRange    ( uint8_t mb, uint8_t * subnet_pos );
    static int          getCidrRange    ( uint8_t mb );
    static inline int   getBlockSize    ( uint8_t mb, uint8_t * subnet_pos )
    {
        return CidrUtils::getCidrRange(mb, subnet_pos);
    }

    static uint8_t      subnetValue     ( ipv4addr_t addr, uint8_t subnet_pos );


    static bool         deAggregate     ( Prefix & p, uint8_t masklen, 
                                          PrefixList & v );


    static bool         matchCidr       ( Prefix & p, ipv4addr_t addr );

    static std::string  ntop            ( ipv4addr_t addr );
    static int          pton            ( const std::string & ipstr, ipv4addr_t & addr );


    static std::string  getHostName();
    static std::string  getHostName     ( ipv4addr_t addr );


    static ipv4addr_t   getHostAddr();
    static ipv4addr_t   getHostAddr     ( const std::string & host );

    static void         getHostAddrList ( const std::string & host, 
                                          AddrList & addrlist );

    static bool         isLoopback      ( ipv4addr_t addr );

};

} // namespace


#endif  // _TCANETPP_CIDRUTILS_H_

