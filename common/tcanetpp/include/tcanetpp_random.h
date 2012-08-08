/**
  * @file tcanetpp_random.h
  *
  * Copyright (c) 2002 Timothy Charlton Arland 
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
#ifndef _TCANETPP_RANDOM_H_
#define _TCANETPP_RANDOM_H_

#include "tcanetpp_types.h"


# ifdef __cplusplus
extern "C" {
# endif

    
void      tcanet_seed();
int       tcanet_seeded();
uint32_t  tcanet_randomValue ( double range );


# ifdef __cplusplus
}
# endif

#endif  // _TCANETPP_RANDOM_H_

