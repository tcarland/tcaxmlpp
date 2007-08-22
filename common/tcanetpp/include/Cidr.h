/**  
  *  The Cidr class is a collection of static utility functions for 
  *  manipulating IP Address, Prefix, and Cidr's.
  *
  *  @Author  tcarland@gmail.com
  *  @Versoin 0.1
 **/
#ifndef CIDR_H_
#define CIDR_H_

#define <string>

#define "tcanet_types.h"


namespace tcanetpp {


class Cidr {
	
  public:

    /**  Determines the validity of a the given cidr prefix by
      *  verifying that address itself is a base prefix.
     **/ 
    static bool         isBasePrefix ( ipv4addr_t addr, uint8_t mb );

    /**  Converts the provided address to a prefix.
      *  Eg. 172.16.6.1/16 converts to 172.16.0.0
     **/
    static ipv4addr_t   toBasePrefix ( ipv4addr_t addr, uint8_t mb );

    /**  Converts the provided address to the dotted-quad string
      *  representation excluding any mask or CIDR style notation.
     **/
    static std::string  toString     ( ipv4addr_t addr );


    /**@{
      *  Converts the provided address and masklen to a CIDR 
      *  style address string.
      *  Eg. aaa.bbb.ccc.ddd/yy
     **/
    static std::string  toCidrString ( ipv4addr_t addr, uint8_t mb );
    static std::string  toCidrString ( const Prefix & P );
    /*@}*/
    

    /**  Converts the provided string to a network ordered unsigned long.
      *  Essentially a wrapper for inet_pton, the method returns 1 on a
      *  successful conversion and 0 on error.
     **/
    static int          StringToAddr ( const std::string & addrStr, 
                                       ipv4addr_t & addr );

    static Prefix       StringToCidr ( const std::string & cidrStr );


    /**  Determines the number of host bits within a single octet for 
      *  any provided masklen. This is used by the base functions. 
      *  Eg. the number of host bits for a /19 is 5 NOT 13.
     **/
    static uint8_t      getSubnetHostBits ( uint8_t netbits );
    static uint8_t      getHostBits       ( ipv4addr_t addr );
    static uint8_t      getNetworkBits    ( ipv4addr_t addr );


    /**  Converts a subnet maskbit count to the equivelent IP Address
      *  (ie. 255.255.252.0).
     **/
    static ipv4addr_t   bitsToMask ( uint8_t mb );
    static uint8_t      maskToBits ( ipv4addr_t addr );


    /**@{
      *  Determines the range of addresses (block size) for a given mask,
      *  and if provided, the starting octet position of the host portion
      *  of the address.
     **/
    static int          getCidrRange ( uint8_t mb, uint8_t * subnet_pos );
    static int          getCidrRange ( uint8_t mb );


    static inline int   getBlockSize ( uint8_t mb, uint8_t * subnet_pos )
    {
        return Cidr::getCidrRange(mb, subnet_pos);
    }
    /*@}*/


    /**  Returns the value of the requested octet in the provided address.
      *  
      *  @param  addr  is the address from which the octet is extracted.
      *  @param subnet_pos  represents the subnet position requested.
     **/
    static uint8_t      subnetValue  ( ipv4addr_t addr, uint8_t subnet_pos );


    /**  De-aggregates the provided Cidr address to the requested masklen
      *  level, populating the provided vector with appropriate cidr's.
      *  For example, a Cidr of 172.16.0.0/19 converted to /24's would 
      *  result in 8 Cidr's being pushed onto the provided vector.
      *  Not very pretty, but functional for now.
     **/
    static bool         deAggregate  ( Prefix & p, uint8_t masklen, 
                                       PrefixList & v );


    /**  Determines whether the provided uint32_t address falls within
      *  the provided Prefix's address scope.
     **/
    static bool         matchCidr    ( Prefix & p, ipv4addr_t addr );


    /**  Simple wrapper for inet_ntop() or inet_pton */
    static std::string  ntop         ( ipv4addr_t addr );
    static ipv4addr_t   pton         ( const std::string & ipstr );


    /**  Returns the localhosts configured hostname */
    static std::string  getHostName();

    /**  Returns the hostname associated with a given address */
    static std::string  getHostName  ( ipv4addr_t addr );


    /**  Returns the localhost's ip address as determined from a lookup
      *  from the resolved hostname obtained from a call to 'gethostname'.
      *  It is not uncommon for a system's '/etc/hosts' file to have a
      *  hostname entry mapped to the loopback address, so results should 
      *  be checked if this method is used to obtain a public ip.
     **/
    static ipv4addr_t   getHostAddr();

    /**  Returns the first or primary ip address of the the given host */
    static ipv4addr_t   getHostAddr ( const std::string & host );

    /**  Returns the list of addresses that a hostname resolves too */
    static void         getHostAddrList ( const std::string & host, 
                                          AddrList & addrlist );

    /**  Convenience method to simply return true if the provided address
      *  equals the loopback address of '127.0.0.1'.
     **/
    static bool isLoopback ( ipv4addr_t addr );

};

} // namespace


#endif  // CIDR_H_

