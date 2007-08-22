#ifndef TCANET_TYPES_H_
#define TCANET_TYPES_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
#include <inttypes.h>
#include <unistd.h>
#include <netinet/in.h>
#endif


#ifdef WIN32  // Typdefs we need for win32 

// stdint.h stuff
typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;

typedef long ssize_t;
typedef int socklen_t;

typedef unsigned long ipv4addr_t;  // ipaddr

#define INET_ADDRSTRLEN 16         // ip str length


#else  // Typedefs we need for unix

typedef uint32_t ipv4addr_t;

#endif


/* for cidr addr str lengths */
#define CIDR_ADDRSTRLEN 20


/* A struct used to represent an Internet CIDR address consisting of the
 * typical format of  "a.b.c.d/e"  where a.b.c.d is the network order
 * unsigned 32 bit integer, and "e" being the corresponding length in mask bits.
 */
typedef struct cidrAddr {
    ipv4addr_t    addr;
    uint16_t      addrlen;
} cidr_t;


/*  for convenience when dealing with these */
typedef struct in_addr inaddr_t;



# ifdef __cplusplus
}  // extern C
# endif


#endif /*TCANET_TYPES_H_*/
