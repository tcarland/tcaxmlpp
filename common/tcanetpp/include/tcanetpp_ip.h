/** 
  * @file tcanetpp_ip.h
  *
  *   Defines various IP headers.
  * These are defined within this library to avoid having to 
  * use platform specific defines for various headers. This also
  * means we avoid the issue of some fields varying slightly in
  * convention across platforms.
  *
  * Copyright (c) 2010 Timothy Charlton Arland
  * @author  tcarland@gmail.com
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



// ----------------------------------------------------------------------
//   IPV6 Address

/**  An IPv6 address. We redefine 'in6addr_t' as two uint64's
  *  adding comparators. It's also a convenient way of
  *  handling, manipulating, or representing ipv6 addresses
  *  in various network programming scenarios.
 **/
typedef struct ipv6addr {
    uint64_t  a;
    uint64_t  b;

    ipv6addr() : a(0), b(0) {}

    ipv6addr ( uint64_t a_, uint64_t b_)
        : a(a_), b(b_)
    {}

    ipv6addr ( const in6addr_t & addr )
    {
        a  = ((const uint64_t*) &addr)[0];
        b  = ((const uint64_t*) &addr)[1];
    }

    bool operator== ( const struct ipv6addr & addr ) const
    {
        return( (a == addr.a) && (b == addr.b) );
    }

    bool operator< ( const struct ipv6addr & addr ) const
    {
        if ( a == addr.a )
            return(b < addr.b);
        return( a < addr.a );
    }

    in6addr_t toAddr() const
    {
        return( *((in6addr_t*) this) );
    }
} ipv6addr_t;



// ----------------------------------------------------------------------
//  Ethernet header


/**  Ethernet address as redefined from net/ethernet.h */
typedef struct EtherAddr {
    uint8_t  ether_octet[ETHER_ADDRLEN];
} ethaddr_t;

/**  The Ethernet header definition */
typedef struct EthHeader {
    uint8_t  dsthost[ETHER_ADDRLEN];
    uint8_t  srchost[ETHER_ADDRLEN];
    uint16_t ethtype;
} neteth_h;



// ----------------------------------------------------------------------
//  IPV4 Header


/**  The IP header definition redefined from netinet/ip.h */
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


#define IP_HDRLEN(ip)   ( ((ip)->version) & 0x0f)
#define IP_VERSION(ip)  ( ((ip)->version) >> 4)



// ----------------------------------------------------------------------
//  IPV6 Header


#define IPV6_NXTHDR_HOPBYHOP 0x00
#define IPV6_NXTHDR_ICMPV4   0x01
#define IPV6_NXTHDR_IGMPV4   0x02
#define IPV6_NXTHDR_IPIP     0x04
#define IPV6_NXTHDR_TCP      0x06
#define IPV6_NXTHDR_EGP      0x08
#define IPV6_NXTHDR_UDP      0x11
#define IPV6_NXTHDR_IPV6     0x29
#define IPV6_NXTHDR_ROUTE    0x2b
#define IPV6_NXTHDR_FRAG     0x2c
#define IPV6_NXTHDR_RSVP     0x2e
#define IPV6_NXTHDR_ESP      0x32
#define IPV6_NXTHDR_AH       0x33
#define IPV6_NXTHDR_ICMPV6   0x3a
#define IPV6_NXTHDR_NONE     0x3b
#define IPV6_NXTHDR_DSTOPT   0x3c



typedef struct Ip6Header {
    uint32_t    flow;       /* 4 bits version, 8 bits TC, 20 bits flowid */
    uint16_t    length;
    uint8_t     nexthdr;
    uint8_t     hoplimit;
    ipv6addr_t  srcaddr;
    ipv6addr_t  dstaddr;

    Ip6Header()
        : flow(0x60),
          length(0),
          nexthdr(0),
          hoplimit(0)
    {}
} netip6_h;


// ----------------------------------------------------------------------
//  UDP Header


/**  The UDP Header definition redefined from netinet/udp.h. */
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


// ----------------------------------------------------------------------
// TCP header


#define TCP_FLAG_FIN    0x01
#define TCP_FLAG_SYN    0x02
#define TCP_FLAG_RST    0x04
#define TCP_FLAG_PUSH   0x08
#define TCP_FLAG_ACK    0x10
#define TCP_FLAG_URG    0x20
#define TCP_FLAG_ECE    0x40
#define TCP_FLAG_CWR    0x80
#define TCP_FLAGS       (TCP_FLAG_FIN|TCP_FLAG_SYN|TCP_FLAG_RST|TCP_FLAG_PUSH|TCP_FLAG_ACK|TCP_FLAG_URG)
#define TCP_OFFSET(tcp) (((tcp)->offset & 0xf0) >> 4)


enum TcpState {
    TCP_STATE_ESTABLISHED = 1,
    TCP_STATE_SYN_SENT,
    TCP_STATE_SYN_RECV,
    TCP_STATE_FIN_WAIT1,
    TCP_STATE_FIN_WAIT2,
    TCP_STATE_TIME_WAIT,
    TCP_STATE_CLOSE,
    TCP_STATE_CLOSE_WAIT,
    TCP_STATE_LAST_ACK,
    TCP_STATE_LISTEN,
    TCP_STATE_CLOSING
};

/** The TCP Header definition redefined from netinet/tcp.h */
typedef struct TcpHeader {
    uint16_t    srcport;
    uint16_t    dstport;
    uint32_t    seq;
    uint32_t    ack_seq;
    uint8_t     offset;
    uint8_t     flags;
    uint16_t    win;
    uint16_t    chksum;
    uint16_t    urgp;

    TcpHeader()
        : srcport(0), dstport(0),
          seq(0), ack_seq(0),
          offset(0), flags(0),
          win(0), chksum(0), urgp(0)
    {}

} nettcp_h;


// ----------------------------------------------------------------------
//  ICMP Header


//  ICMP types and codes 
// We redefine these for simplifying platform compatibility
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
#define ICMP_EXC_TTL            0       /* TTL count exceeded  */
#define ICMP_EXC_FRAGTIME       1       /* Fragment reassemble time exceeded */


/**  The ICMP Header definition redefined from netinet/ip_icmp.h
 *   This is a weak translation and ip_icmp.h header should be preferred.
 **/
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


// ----------------------------------------------------------------------
//  IGMP Header

#define IGMP_TYPE_MEMBERQUERY   0x11
#define IGMP_TYPE_REPORTV1      0x12
#define IGMP_TYPE_DVMRP         0x13
#define IGMP_TYPE_PIM           0x14
#define IGMP_TYPE_TRACE         0x15
#define IGMP_TYPE_REPORTV2      0x16
#define IGMP_TYPE_LEAVEGRPV2    0x17
#define IGMP_TYPE_MTRACERESP    0x1e
#define IGMP_TYPE_MTRACE        0x1f

#define IGMP_TYPE_MAXHOSTDELAY  10
#define IGMP_TYPE_TIMERSCALE    10

/* IGMP v2 state table */
#define IGMP_STATE_DELAY  1
#define IGMP_STATE_IDLE   2
#define IGMP_STATE_LAZY   3
#define IGMP_STATE_SLEEP  4
#define IGMP_STATE_AWAKE  5

#define IGMP_ROUTER_V1    1
#define IGMP_ROUTER_V2    2


typedef struct IgmpHeader {
    uint8_t     type;
    uint8_t     code;
    uint16_t    chksum;
    ipv4addr_t  grpaddr;

    IgmpHeader()
        : type(0), code(0), chksum(0),
          grpaddr(0)
    {}
} netigmp_h;



#endif  // _TCANETPP_TCANETIP_H_

