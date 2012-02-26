/** 
  * @file tcanetpp_types.h
  *
  *   This header provides various defintions and typedefs that help to ensure 
  * cross-platform compilation, primarily between Unix and Win32.
  *   
  * Copyright (c) 2002,2008 Timothy Charlton Arland 
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
#ifndef _TCANETPP_TYPES_H_
#define _TCANETPP_TYPES_H_

#include "tcanetpp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
# include <Winsock2.h>
# include <ws2tcpip.h>
# include <time.h>
#else
# include <inttypes.h>
# include <unistd.h>
# include <netinet/in.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netdb.h>
#endif


// not relying on win32 inttypes, so we define our own.
#  ifdef WIN32
typedef long                     ssize_t;
typedef signed char              int8_t;
typedef unsigned char            uint8_t;
typedef short                    int16_t;
typedef unsigned short           uint16_t;
typedef int                      int32_t;
typedef unsigned long            uint32_t;
typedef long long                int64_t;
typedef unsigned long long       uint64_t;
typedef int                      socklen_t;
#  endif


// win32 socket descriptor and storage
//
#  ifdef WIN32
typedef SOCKET                   sockfd_t;
typedef SOCKADDR_STORAGE         sockaddr_t;
#  else
typedef int                      sockfd_t;
typedef struct sockaddr_storage  sockaddr_t;
#  endif


// some commonly used defines and types
//
#define INET4ADDRSTRLEN         INET_ADDRSTRLEN;
#define INET6ADDRSTRLEN         INET6_ADDRSTRLEN;
#define INET4_CIDRSTRLEN        20
#define ERRORSTRLEN             256
#define MAXMASKLEN              32
#define MAXMASKLEN_IPV6         64   // nothing should subnet between 64 and 128
#define IPV4ADDR_LOOPBACK       16777343
#define ETHER_ADDRLEN           6

// some handy typedef's
typedef struct in_addr          inaddr_t;
typedef struct in6_addr         in6addr_t;
typedef uint32_t                ipv4addr_t;
typedef uint64_t                evid_t;       // event registration id


/** Structure defining an IPV4 Address and its associated mask
 *  length in bits representing the a.b.c.d/mb cidr format.
 *  This definition exists here vs _ip header for the
 *  extern C support since it is used within the patricia.
 **/
typedef struct ipv4cidr {
    ipv4addr_t   addr;
    uint16_t     mb;
} cidr_t;


# ifdef __cplusplus
}  // extern C
# endif


#endif // _TCANETPP_TYPES_H_

