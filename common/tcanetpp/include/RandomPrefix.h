/**  
  * @file RandomPrefix.h
  *
  * Copyright (c) 2002 Timothy Charlton Arland 
  * @author  tca@charltontechnology.net
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
#ifndef _TCANETPP_RANDOMPREFIX_H_
#define _TCANETPP_RANDOMPREFIX_H_


#include "Prefix.hpp"
#include "random.h"

namespace tcanetpp {
    

/** Random number and prefix generation */
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

