/**
  * @file byte_order.h
  *
  *   htonq() and ntohq() macros for handling uint64_t
  * or unsigned long long's.
  *
  * Copyright (c) 2002 Timothy Charlton Arland 
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
#ifndef _TCANETPP_BYTEORDER_H_
#define _TCANETPP_BYTEORDER_H_

#include <sys/types.h>
#include <sys/param.h>


#ifdef BSD
# include <machine/endian.h>
#elif __sparc
# include <sys/byteorder.h>
#else
# include <endian.h>
#endif


#if( BYTE_ORDER == LITTLE_ENDIAN )
# define htonll(x) ( ( ((uint64_t)htonl((uint32_t)((x) >> 32))) ) | \
  ( ((uint64_t)htonl((uint32_t)((x) & 0x00000000ffffffff)) << 32) ) )
# define ntohll(x) (htonll(x))
# define htonf(x) (htonl((uint32_t)x))
# define ntohf(x) (ntohl((uint32_t)x))
#else
# define htonll(x) ((uint64_t)(x))
# define ntohll(x) ((uint64_t)(x))
# define htonf(x)  (x)
# define ntohf(x)  (x)
#endif

#define htonq(x) htonll(x)
#define ntohq(x) ntohll(x)


#endif  // _TCANETPP_BYTEORDER_H_

