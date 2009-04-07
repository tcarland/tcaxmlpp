/**
  *   A circular buffer implementation which allows for direct access and 
  *  interaction to the underlying buffer to avoid excessive copying.
  *
  * Copyright (c) 2008, 2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
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

#include <string>

#include "Exception.hpp"


namespace tcanetpp {


/*  Default size limitations. */
#define MIN_CIRBUFFER_SIZE       1
#define MAX_CIRBUFFER_SIZE       (4294967295U)
#define DEFAULT_CIRBUFFER_SIZE   (1024000)


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

class CircularBuffer {

  public:

    CircularBuffer ( size_t totalsize = DEFAULT_CIRBUFFER_SIZE ) 
        throw ( BufferException );

    CircularBuffer ( const CircularBuffer & buffer ) 
        throw ( BufferException );

    ~CircularBuffer();


    size_t         read    ( void       * buff, size_t n );
    size_t         write   ( const void * buff, size_t n );
 
    size_t         reverse ( size_t offset );
    size_t         skip    ( size_t offset );
    bool           resize  ( size_t buffsize ) throw ( BufferException );

    
    size_t         readAvailable() const;
    size_t         readPtrAvailable() const;
    size_t         writeAvailable() const;
    size_t         writePtrAvailable() const;

    bool           isWrapped() const;
   

    void           clear();
    inline  void   reset()       { return this->clear(); }
    inline size_t  size() const  { return this->_buffsize; }


  public:

    char*          getWritePtr ( size_t * size );
    void           setWritePtr ( size_t   offset ) throw ( BufferException );

    char*          getReadPtr  ( size_t * size );
    void           setReadPtr  ( size_t   offset ) throw ( BufferException );

    const 
    std::string&   getErrorStr() const;

    static const
    std::string&   Version();


  protected:

    void           init() throw ( BufferException );


  private:

    char*                   _buffer;    
    char*                   _readPtr;
    char*                   _writePtr;
    char*                   _endPtr;   
    char*                   _wrapPtr;   

    size_t                  _buffsize;
    std::string             _errstr;

    static std::string      _Version;
};


}  // namespace


#endif //  _TCANETPP_CIRCULARBUFFER_H_

