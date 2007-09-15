/**  Tools for random number and prefix generation. */
#ifndef _TCANETPP_RANDOMPREFIX_H_
#define _TCANETPP_RANDOMPREFIX_H_


#include "Prefix.hpp"
#include "random.h"

namespace tcanetpp {
    

/**  A wrapper to random.h which allows the passing of a
  *  libtcanet Prefix object. This is mostly for convenience. 
 **/
class Random {

  public:

    /** Returns a pseudo-random positive (unsigned) integer by way of rand() */
    static
    uint32_t    RandomValue   ( double range );

    static
    Prefix*     RandomPrefix  ( Prefix * agg );

    static 
    ipv4addr_t  RandomPrefix  ( ipv4addr_t agg, uint8_t prefix_mb );

  protected:

    static bool  _seeded;

};

} // namespace


#endif  // _TCANETPP_RANDOMPREFIX_H_
