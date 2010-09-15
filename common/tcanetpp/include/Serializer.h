/**
  * @file Serializer.h
  *
  *   Serialization functions which can be used by objects that are 
  * 'Serializable'.
  *  
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
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
#ifndef _TCANETPP_SERIALIZER_H_
#define _TCANETPP_SERIALIZER_H_

extern "C" {
# include <string.h>
}
#include <string>

#include "tcanetpp_types.h"


namespace tcanetpp {


#define SERIALIZER_WORDSIZE 4


/**  A utility class defining Pack and Unpack functions for serializing 
  *  data and ensuring proper byte alignment and padding. 
 **/
class Serializer {

  public:

    static int  Pack   ( char * buffer, size_t buflen, 
                         const char * val, size_t val_len );

    static int  Pack   ( char * buffer, size_t buflen,
                         const std::string & val );

    static int  Pack   ( char * buffer, size_t buflen, int8_t   val );
    static int  Pack   ( char * buffer, size_t buflen, uint8_t  val );
    static int  Pack   ( char * buffer, size_t buflen, int16_t  val );
    static int  Pack   ( char * buffer, size_t buflen, uint16_t val );
    static int  Pack   ( char * buffer, size_t buflen, int32_t  val );
    static int  Pack   ( char * buffer, size_t buflen, uint32_t val );
    static int  Pack   ( char * buffer, size_t buflen, int64_t  val );
    static int  Pack   ( char * buffer, size_t buflen, uint64_t val );

    //---------------------------------------------------------------------

    static int  Unpack ( const char  * buffer, size_t buflen, 
                         char        * val,    size_t val_len, 
                         size_t      & val_written );

    static int  Unpack ( const char  * buffer, size_t buflen,
                         std::string & val );

    static int  Unpack ( const char  * buffer, size_t buflen, int8_t   & val );
    static int  Unpack ( const char  * buffer, size_t buflen, uint8_t  & val );
    static int  Unpack ( const char  * buffer, size_t buflen, int16_t  & val );
    static int  Unpack ( const char  * buffer, size_t buflen, uint16_t & val );
    static int  Unpack ( const char  * buffer, size_t buflen, int32_t  & val );
    static int  Unpack ( const char  * buffer, size_t buflen, uint32_t & val );
    static int  Unpack ( const char  * buffer, size_t buflen, int64_t  & val );
    static int  Unpack ( const char  * buffer, size_t buflen, uint64_t & val );

    //---------------------------------------------------------------------
    
    static inline size_t PadLen ( const size_t & wtlen )
    {
        size_t  pad = wtlen % SERIALIZER_WORDSIZE;
        return (pad == 0) ? 0 : SERIALIZER_WORDSIZE - pad;
    }

    //----------------------------------------------------------------------

    static inline size_t Skip ( const size_t & length )
    {
        return Serializer::PadLen(length);
    }

    //----------------------------------------------------------------------

    static inline size_t Pad ( char * buffer, size_t  buflen,
                               const size_t & wtlen )
    {
        size_t  pad = Serializer::PadLen(wtlen);

        if ( pad > buflen )
            return buflen;

        return pad;
    }

};


}  // namespace


#endif  //  _TCANETPP_SERIALIZER_H_

