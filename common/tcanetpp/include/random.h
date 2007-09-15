#ifndef _TCANETPP_RANDOM_H_
#define _TCANETPP_RANDOM_H_

#include "tcanetpp_types.h"


# ifdef __cplusplus
extern "C" {
# endif

    
void      tcanet_seed();
uint32_t  tcanet_randomValue ( double range );


# ifdef __cplusplus
}
# endif

#endif  // _TCANETPP_RANDOM_H_

