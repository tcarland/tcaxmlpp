/**
  * htonq() and ntohq() macros for handling uint64_t
  *   or unsigned long long's.
  *
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

