#ifndef _foo_h
#define _foo_h


#include "tcanetpp_types.h"


typedef struct foo {
    uint32_t    client;
    uint32_t    timestamp;
    uint32_t    count;

    foo() : client(0), timestamp(0), count(0) {}
} foo_t;


#endif

