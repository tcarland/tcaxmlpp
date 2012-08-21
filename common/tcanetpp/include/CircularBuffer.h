/**
  * @file CircularBuffer.h
  *
  *   A circular buffer implementation which allows for direct access and 
  *  interaction to the underlying buffer. This was originally designed 
  *  to read inbound udp fast while avoiding an additional memcpy's that 
  *  occur in a std read/write function calls. The get/set ptr functions 
  *  should be used with caution.
  *
  * Copyright (c) 2001 Timothy Charlton Arland 
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
#ifndef _TCANETPP_CIRCULARBUFFER_H_
#define _TCANETPP_CIRCULARBUFFER_H_


#ifdef WIN32
#include <Winsock2.h>
#pragma warning (disable:4290)
#else
extern "C" {
# include <unistd.h>
}
#endif

#include "Exception.hpp"


namespace tcanetpp {


/*  Default size limitations. */
#define MIN_CIRBUFFER_SIZE       1
#define MAX_CIRBUFFER_SIZE       (4294967295U)
#define DEFAULT_CIRBUFFER_SIZE   (1024000U)
#define CIRBUFFER_VERSION        "v1.98"


// ----------------------------------------------------------------------
/**  Buffer Exception class
  *   Thrown in 'set' methods if buffer overwrite or overread
  *   is attempted.
 **/

class BufferException : public Exception {
  public:
    BufferException ( const std::string & s_err ) 
        : Exception(s_err) 
    {}
};


// ----------------------------------------------------------------------
/**  The CircularBuffer implementation is primarily intended as a packet
  *  buffer and was conceived for use with reading UDP datagrams rapidly.
  *  It allows direct access to the underlying char* for direct writing and
  *  reading saving multiple memcpy's that result from normal read()/writes().
  *  It is also useful to use the safe read()/write() methods which guarantee
  *  safe operation as well as truly treating the buffer as circular. This 
  *  use case as a packet buffer is intended to allow the buffer to hold many
  *  datagrams efficiently at once for bulk reads in larger buffering scenarios 
  *  and unlike most buffers, it is truly circular, allowing writes to wrap and
  *  essentially trail the read position.
 **/
class CircularBuffer {

  public:

    CircularBuffer ( size_t totalsize = DEFAULT_CIRBUFFER_SIZE ) 
        throw ( BufferException );

    CircularBuffer ( const CircularBuffer & buffer ) 
        throw ( BufferException );

    ~CircularBuffer();


    size_t         read    ( void       * buff, size_t n );
    size_t         write   ( const void * buff, size_t n );
 
    size_t         readAvailable() const;
    size_t         writeAvailable() const;

    size_t         reverse ( size_t offset );
    size_t         skip    ( size_t offset );

    bool           resize  ( size_t buffsize ) throw ( BufferException );

    void           clear();
    inline  void   reset()       { return this->clear(); }
    inline size_t  size() const  { return this->_buffsize; }


  public:

    char*          getWritePtr ( size_t * size );
    void           setWritePtr ( size_t   offset ) throw ( BufferException );

    char*          getReadPtr  ( size_t * size );
    void           setReadPtr  ( size_t   offset ) throw ( BufferException );
    
    size_t         readPtrAvailable() const;
    size_t         writePtrAvailable() const;

    static
    const char*    Version();


  protected:

    void           init() throw ( BufferException );
    bool           isWrapped() const;


  private:

    char*          _buffer;    
    char*          _readPtr;
    char*          _writePtr;
    char*          _endPtr;   
    char*          _wrapPtr;   

    size_t         _buffsize;

};

}  // namespace


#endif //  _TCANETPP_CIRCULARBUFFER_H_

