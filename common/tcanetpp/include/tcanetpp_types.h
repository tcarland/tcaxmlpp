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

#ifdef __cplusplus
extern "C" {
#endif



#ifdef WIN32
# include <Winsock2.h>
# include <ws2tcpip.h>
#else
# include <inttypes.h>
# include <unistd.h>
# include <netinet/in.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/socket.h>
#endif


// not relying on win32 inttypes, so we define our own.
#ifdef WIN32  

  typedef long                 ssize_t;
  typedef signed char          int8_t;
  typedef unsigned char        uint8_t;
  typedef short                int16_t;
  typedef unsigned short       uint16_t;
  typedef int                  int32_t;
  typedef unsigned             uint32_t;
  typedef long long            int64_t;
  typedef unsigned long long   uint64_t;
 
  typedef int                  socklen_t;
  typedef unsigned long        ipv4addr_t;
  typedef SOCKET               sockfd_t;
  
#else  

  typedef uint32_t             ipv4addr_t;
  typedef int                  sockfd_t;

#endif


// some commonly used defines and types
//
#define INET_CIDRSTRLEN   20
#define ERRORSTRLEN       256
#define MAXMASKLEN        32
#define IPV4ADDR_LOOPBACK 16777343
 

typedef struct in_addr    inaddr_t;


/** Struct defining an IP Address and its associated Mask 
 *  length in bits representing the a.b.c.d/mb cidr format.
 *  We use a short for the addr length to keep alignment.
 **/
typedef struct cidr {
    ipv4addr_t   addr;
    uint16_t     addrlen;
} cidr_t;


# ifdef __cplusplus
}  // extern C
# endif


#endif // _TCANETPP_TYPES_H_

