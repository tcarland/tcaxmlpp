/** 
  * @file tcanetpp_ip.h
  *
  *   Defines our IP headers for use with raw sockets.
  *
  * Copyright (c) 2010 Timothy Charlton Arland
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
#ifndef _TCANETPP_TCANETIP_H_
#define _TCANETPP_TCANETIP_H_

#include "tcanetpp_types.h"


/**  The IP header defined for use within this lib to avoid 
  *  performing platform specific defines since the header 
  *  and its fields vary slightly in convention across systems
 **/
typedef struct IpHeader {
    uint8_t   version;
    uint8_t   tos;
    uint16_t  length;
    uint16_t  id;
    uint16_t  frag;
    uint8_t   ttl;
    uint8_t   protocol;
    uint16_t  chksum;
    uint32_t  srcaddr;
    uint32_t  dstaddr;

    IpHeader() 
        : version(0x45), tos(0), 
          length(0), id(0), 
          frag(0), ttl(0), chksum(0), 
          srcaddr(0), dstaddr(0)
    {}

} netip_h;

/**  The UDP Header definition. */
typedef struct UdpHeader {
    uint16_t  srcport;
    uint16_t  dstport;
    uint16_t  length;
    uint16_t  chksum;

    UdpHeader() 
        : srcport(0), dstport(0), 
          length(0),  chksum(0)
    {}

} netudp_h;

/**  The ICMP Header definition */
typedef struct IcmpHeader {
    uint8_t  type;
    uint8_t  code;
    uint16_t chksum;
    uint16_t id;
    uint16_t seq;

    IcmpHeader()
        : type(0), code(0), chksum(0), 
          id(0), seq(0)
    {}

} neticmp_h;


//  ICMP types and codes 
// We redefine these for simplifying platform compatability
#define ICMP_ECHOREPLY          0
#define ICMP_DEST_UNREACH       3
#define ICMP_SOURCE_QUENCH      4 
#define ICMP_REDIRECT           5
#define ICMP_ECHO               8
#define ICMP_TIME_EXCEEDED      11
#define ICMP_PARAMETERPROB      12
#define ICMP_TIMESTAMP          13
#define ICMP_TIMESTAMPREPLY     14
#define ICMP_INFO_REQUEST       15
#define ICMP_INFO_REPLY         16
#define ICMP_ADDRESS            17
#define ICMP_ADDRESSREPLY       18
#define NR_ICMP_TYPES           18


/* Codes for UNREACH. */
#define ICMP_NET_UNREACH        0
#define ICMP_HOST_UNREACH       1
#define ICMP_PROT_UNREACH       2
#define ICMP_PORT_UNREACH       3
#define ICMP_FRAG_NEEDED        4
#define ICMP_SR_FAILED          5 
#define ICMP_NET_UNKNOWN        6
#define ICMP_HOST_UNKNOWN       7
#define ICMP_HOST_ISOLATED      8
#define ICMP_NET_ANO            9
#define ICMP_HOST_ANO           10
#define ICMP_NET_UNR_TOS        11
#define ICMP_HOST_UNR_TOS       12
#define ICMP_PKT_FILTERED       13  
#define ICMP_PREC_VIOLATION     14 
#define ICMP_PREC_CUTOFF        15
#define NR_ICMP_UNREACH         15

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET          0  
#define ICMP_REDIR_HOST         1 
#define ICMP_REDIR_NETTOS       2
#define ICMP_REDIR_HOSTTOS      3

/* Codes for TIME_EXCEEDED. */
#define ICMP_EXC_TTL            0       /* TTL count exceeded           */
#define ICMP_EXC_FRAGTIME       1       /* Fragment Reass time exceeded */


#endif  // _TCANETPP_TCANETIP_H_

