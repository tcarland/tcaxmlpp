/**
  *   CircularBuffer.cpp
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
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
#define _TCANETPP_CIRCULARBUFFER_CPP_

extern "C" {
# include <errno.h>
}

#include <cstdlib>
#include <cstdio>

#include "CircularBuffer.h"
#include "StringUtils.h"


namespace tcanetpp {

std::string 
CircularBuffer::_Version = "$Id: CircularBuffer.cpp 4.2 2008/08/18  tca";


// ----------------------------------------------------------------------

/**
  *   Class Constructor for a circular buffer instance. 
  *  Throws an exception if the underlying malloc fails or the
  *  buffer size is out of range. If no size is given,
  *  a default is used.
 **/
CircularBuffer::CircularBuffer ( size_t totalsize ) throw ( BufferException )
    : _buffer(NULL),
      _buffsize(totalsize)
{
    this->init();
}

/**  Performs a full copy of the buffer including the current
  *  read/write/wrap state.
 **/
CircularBuffer::CircularBuffer ( const CircularBuffer & buffer ) 
    throw ( BufferException )
{
    _buffsize = buffer._buffsize;

    this->init();

    ::memcpy(this->_buffer, buffer._buffer, _buffsize);

    if ( buffer._endPtr != NULL ) {
        _endPtr   = _buffer;
        _endPtr  += (buffer._endPtr - buffer._buffer);
        _wrapPtr  = _endPtr;
    }

    _readPtr  += (buffer._readPtr - buffer._buffer);
    _writePtr += (buffer._writePtr - buffer._buffer);
}


CircularBuffer::~CircularBuffer()
{
    if ( _buffer )
        ::free(_buffer);
}

// ----------------------------------------------------------------------

void
CircularBuffer::init() throw ( BufferException )
{
    if ( _buffer )
        ::free(_buffer);

    if ( _buffsize < MIN_CIRBUFFER_SIZE || _buffsize > MAX_CIRBUFFER_SIZE ) {
        std::string err = "CircularBuffer::init() Invalid Buffer size: ";
        err.append(StringUtils::toString(_buffsize));
	throw BufferException(err);
    }

    _buffer = (char*) ::calloc(_buffsize, sizeof(char));

    if ( _buffer == NULL )
	throw BufferException("Failed to allocate (calloc) buffer");
    
    _readPtr  = _buffer;
    _writePtr = _buffer;
    _endPtr   = NULL;
    _wrapPtr  = NULL;

    return;
}

// ----------------------------------------------------------------------

    /**   Reads <n> bytes from the circular buffer to the
      *   provided buffer, returning the number of bytes
      *   actually copied to the buffer.
      *
      *   @param buff  the buffer to fill.
      *   @param n     the number of bytes to copy. 
     **/

size_t
CircularBuffer::read ( void *buff, size_t n )
{
    char   *rptr, *ptr;
    size_t  size, rn, rd;
   
    ptr = (char*) buff;
    rd  = n;

    while ( rd > 0 && ptr != NULL ) {
	if ( (rptr = getReadPtr(&size)) == NULL ) 
	    break;

	if ( size > rd )
	    rn = rd;
	else
	    rn = size;

	::memcpy(ptr, rptr, rn);

	this->setReadPtr(rn);

	rd  -= rn;
	ptr += rn;
    }

    return(n - rd);
}

// ----------------------------------------------------------------------

    /**   Writes <n> bytes from the provided buffer to the 
      *   CirBuffer returning the number of bytes written.
      *
      *   @param buff  the buffer from which to copy.
      *   @param n     the number of bytes to copy.
     **/

size_t
CircularBuffer::write ( const void *buff, size_t n )
{
    char   *wptr, *ptr;
    size_t  size, wn, wd;

    ptr = (char*) buff;
    wd  = n;

    while ( wd > 0 && ptr != NULL ) {
	size = wd;

	if ( (wptr = getWritePtr(&size)) == NULL )
	    break;

	if ( ((size_t)size) >= wd )
	    wn = wd;
	else
	    wn = size;

	::memcpy(wptr, ptr, wn);
        this->setWritePtr(wn);

	wd -= wn;
	ptr += wn;
    }

    return(n - wd);
}

// ----------------------------------------------------------------------

    /**   Reverses the buffer by the provided number of bytes.
      *   NOTE that this is only a safe operation immediatly
      *   following a read and while NO writes have occured.
     **/

size_t
CircularBuffer::reverse ( size_t offset )
{
    size_t  rpos = 0;
    size_t  wrap = 0;

    if ( _endPtr == NULL && _wrapPtr != NULL ) {
        rpos  = (_readPtr - _buffer);
        wrap  = (this->writePtrAvailable() - (_buffsize - (_wrapPtr - _buffer)));
        rpos += wrap;
    } else if ( _endPtr == NULL ) {
        rpos = _readPtr - _buffer;
    } else {
        rpos = _readPtr - _writePtr;
    }


    if ( offset > rpos ) {
	offset = 0;
    } else if ( _endPtr == NULL && offset > (size_t)(_readPtr - _buffer) ) {

        wrap      = offset - (_readPtr - _buffer);

        _endPtr   = _wrapPtr;
        _wrapPtr  = NULL;
        _readPtr  = _endPtr;
        _readPtr -= wrap;
    } else {
        _readPtr -= offset;
    }

    return offset;
}

// ----------------------------------------------------------------------

    /**   Skips the read position forward the provided number of bytes */

size_t
CircularBuffer::skip ( size_t offset )
{
    size_t  sz  = this->readPtrAvailable();

    if ( offset > this->readAvailable() ) {
	offset = 0;
    } else if ( offset > sz ) {
        // allow for wrap
        this->setReadPtr(sz);
        this->setReadPtr(offset-sz);
    } else {
        this->setReadPtr(offset);
    }

    return offset;
}

// ----------------------------------------------------------------------

    /**   Returns the number of bytes available for reading
      *   contiguously (without a buffer wrap).
     **/

size_t
CircularBuffer::readPtrAvailable() const
{
    size_t  size = 0;

    if ( _endPtr == NULL )
        size = _writePtr - _readPtr;
    else
        size = _endPtr - _readPtr;

    return size;
}
    
    /**   Returns the full amount of data available including
      *   wrapped data.
     **/

size_t
CircularBuffer::readAvailable() const
{
    size_t   size = 0;

    if ( _endPtr == NULL )
	size = _writePtr - _readPtr;
    else
	size = (_endPtr - _readPtr) + (_writePtr - _buffer);

    return size;
}

// ----------------------------------------------------------------------

    /**   The amount of contigous space available for writing
      *   without a buffer wrap.
     **/

size_t
CircularBuffer::writePtrAvailable() const
{
    size_t  size = 0;

    if ( _endPtr == NULL )
        size = _buffsize - (_writePtr - _buffer);
    else
        size = _readPtr - _writePtr;

    return size;
}

    /**   The total amount space available for writing including
      *   any 'wrapped' space.
     **/

size_t
CircularBuffer::writeAvailable() const
{
    size_t  size = 0;

    if ( _endPtr == NULL )
        size = (_buffsize - (_writePtr - _buffer)) + (_readPtr - _buffer);
    else 
        size = _readPtr - _writePtr;

    return size;
}

// ----------------------------------------------------------------------

    /**   Returns a boolean indicating the current buffer state */
bool
CircularBuffer::isWrapped() const
{
    if ( _endPtr == NULL )
        return false;

    return true;
}

// ----------------------------------------------------------------------

void
CircularBuffer::clear()
{
    _readPtr  = _buffer;
    _writePtr = _buffer;
    _endPtr   = NULL;
    _wrapPtr  = NULL;

    return;
}

// ----------------------------------------------------------------------

    /**     Resizes the buffer to the requested size. This operation 
      *   may result in the need to 'realign' the data blocks in the 
      *   buffer to allow the resize to occur. The underlying effect 
      *   being a call to memmove and a realloc.
      *     The resize can fail if the size requested is larger than the 
      *   max allowed, or is smaller than an existing data block within 
      *   the buffer. The method can throw an exception if the call 
      *   to realloc fails.
     **/

bool
CircularBuffer::resize ( size_t buffsize ) throw ( BufferException )
{
    if ( buffsize < MIN_CIRBUFFER_SIZE || buffsize > MAX_CIRBUFFER_SIZE) 
        return false;

    if ( buffsize < _buffsize ) {  // buffer size reduction
        if ( this->readAvailable() > buffsize )
            return false;
 
        if ( this->isWrapped() ) { // realign buffer data in wrapped state
            size_t mvamt = _endPtr - _readPtr;
            char * start = _buffer + (buffsize - mvamt);

            ::memmove(start, _readPtr, mvamt);

            _readPtr = start;
            _endPtr  = start + mvamt;

        } else if ( _readPtr != _buffer ) {  // realign buffer if neccessary

            if ( ((size_t)(_writePtr - _buffer)) > buffsize ) {
                size_t mvamt = this->readPtrAvailable();
                
                ::memmove(_buffer, _readPtr, mvamt);

                _readPtr  = _buffer;
                _writePtr = _readPtr + mvamt;
            }
        }
    }

    size_t   roffset, woffset, eoffset;
    bool     wrap;

    //  record  ptr offsets
    wrap     = false;
    roffset  = _readPtr  - _buffer;
    woffset  = _writePtr - _buffer;
    eoffset  = 0;

    if ( this->isWrapped() ) {
        eoffset = _endPtr - _buffer;
        wrap    = true;
    }

    // realloc the buffer
    _buffer   = (char*) ::realloc(_buffer, buffsize);

    if ( _buffer == NULL )
        throw BufferException("CircularBuffer::resize() Error in realloc.");

    // remap ptrs
    _readPtr  = _buffer + roffset;
    _writePtr = _buffer + woffset;
    _wrapPtr  = NULL;
    _endPtr   = NULL;

    if ( wrap )
        _endPtr = _buffer + eoffset;
 
    // shift data block to end
    if ( buffsize > _buffsize && wrap ) {
        size_t mvamt = this->readPtrAvailable();
        char * start = (_buffer + buffsize) - mvamt;

        ::memmove(start, _readPtr, mvamt);

        _readPtr = start;
        _endPtr  = start + mvamt;
    }

    _buffsize = buffsize;

    return true;
}

// ----------------------------------------------------------------------

    /*  DISCLAIMER:
     *
     *  The following methods are primarily used internally, however 
     *  direct use is allowed to provide for faster reads/writes by 
     *  eliminating an extra memcpy. The standard read/write() methods 
     *  above accept a user provided buffer to write to or read from
     *  by performing a memcpy for each operation. The following methods 
     *  allow access to the internal buffer pointers essentially providing 
     *  direct access to the underlying buffer. The methods are reasonably
     *  straightforward, but are only safe as long as the user does not 'lie'.
     *  These methods are most certainly not thread-safe.
     *
     */

// ----------------------------------------------------------------------

    /**   Returns a char* pointing to the first available write location
      *   of the buffer and populates size with the space available for 
      *   writing.
      *
      *   @param size  is the requested size of the buffer needed for
      *   writing. If the requested size is not available, the method
      *   will return NULL.  The size parameter is populated with the
      *   actual size available for writing (ie. total available space
      *   is always provided rather than the minimum requested space.
      *
      *   It's IMPORTANT to note that the size parameter is OVERWRITTEN,
      *   as a potential mistake would be to use the same size value
      *   when calling the subsequent setWritePtr method. 
      *
      *   A 'NULL' is returned on error.
     **/

char*
CircularBuffer::getWritePtr ( size_t * size )
{
    char   *ptr    = NULL;
    size_t  offset = 0;

    if ( size == NULL )
        return ptr;

    offset = this->writePtrAvailable();

    if ( _endPtr == NULL && offset < *size ) {
        offset = _readPtr - _buffer;

        if ( offset < *size ) {
            _errstr = "CircularBuffer::getWritePtr() Insufficient space";
            *size   = 0;
            return NULL;
        }

        // write wrap
        _endPtr   = _writePtr;
        _writePtr = _buffer;
        _wrapPtr  = NULL;

    } else if ( (size_t) offset < *size ) {
	_errstr = "CircularBuffer::getWritePtr() Insufficient space";
	*size   = 0;
	return NULL;
    }

    ptr   = _writePtr;
    *size = offset;

    return ptr;
}

// ----------------------------------------------------------------------

    /**   Accepts @param  offset as the size written to the buffer,
      *   and updates the writePtr to the new position (ie. the 
      *   last position + offset ).
      *
      *   The method will throw a BufferException if the offset
      *   provided is not valid (ie. would cause an overwrite)
     **/

void
CircularBuffer::setWritePtr ( size_t offset ) throw ( BufferException )
{
    if ( offset > this->writePtrAvailable() )
        throw BufferException("CircularBuffer::setWritePtr() Buffer overwrite attempted");

    _writePtr += offset;
    
    if ( (size_t)(_writePtr - _buffer) == _buffsize ) {
        // write wrap
        _endPtr   = _writePtr;
        _writePtr = _buffer;
        _wrapPtr  = NULL;
    }

    return;
}

// ----------------------------------------------------------------------

    /**   Returns the readPtr to the first readable position if
      *   there is space to read, or returns NULL on no data. 
      *   
      *   @param size  is populated with the total size available
      *   for reading.
     **/

char*
CircularBuffer::getReadPtr ( size_t * size )
{
    char    *ptr = NULL;
    size_t   rd;

    if ( size == NULL )
        return ptr;
    
    rd    = this->readPtrAvailable();
    *size = rd;

    if ( rd == 0 && _endPtr != NULL ) {
        // read wrap
        _readPtr = _buffer;
        _wrapPtr = _endPtr;
        _endPtr  = NULL;

        rd    = this->readPtrAvailable();
        *size = rd;
    }
        
    if ( rd > 0 )
        ptr = _readPtr;

    return ptr;
}

// ----------------------------------------------------------------------

    /**   This method sets the read pointer to the new position or
      *   throws an exception if the offset provided would cause an overread.
      *   
      *   @param offset  is the size (bytes) read off the buffer.
     **/

void
CircularBuffer::setReadPtr ( size_t offset ) throw ( BufferException )
{
    if ( offset > this->readPtrAvailable() )
        throw BufferException("CircularBuffer::setReadPtr() Buffer overread attempted!");

    _readPtr += offset;
    
    if ( _endPtr != NULL && _readPtr == _endPtr ) {
        // read wrap
        _readPtr = _buffer;
        _wrapPtr = _endPtr;
        _endPtr  = NULL;
    }

    return;
}

// ----------------------------------------------------------------------

const std::string&
CircularBuffer::getErrorStr() const
{
    return this->_errstr;
}

// ----------------------------------------------------------------------

const std::string&
CircularBuffer::Version()
{
    return CircularBuffer::_Version;
}

// ----------------------------------------------------------------------


} // namespace


//   _TCANETPP_CIRCULARBUFFER_CPP_

