#define _TCANETPP_CIRCULARBUFFER_CPP_

extern "C" {
# include <errno.h>
}

#include <cstdlib>
#include <cstdio>

#include "CircularBuffer.h"


namespace tcanetpp {

std::string 
CircularBuffer::_Version = "$Id: CircularBuffer.cpp,v 3.0 2007/09/16 04:40:57 tca Exp $";


// ----------------------------------------------------------------------

CircularBuffer::CircularBuffer() throw ( BufferException )
    : _buffer(NULL),
      _buffsize(DEFAULT_CIRBUFFER_SIZE)
{
    this->init();
}

CircularBuffer::CircularBuffer ( size_t totalsize ) throw ( BufferException )
    : _buffer(NULL),
      _buffsize(totalsize)
{
    this->init();
}

CircularBuffer::CircularBuffer ( const CircularBuffer & buffer ) throw ( BufferException )
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

    if ( _buffsize < MIN_CIRBUFFER_SIZE || _buffsize > MAX_CIRBUFFER_SIZE )
	throw BufferException("Invalid _buffer size");

    _buffer = (char*) ::calloc(_buffsize, sizeof(char));

    if ( _buffer == NULL )
	throw BufferException("Failed to malloc buffer");
    
    _readPtr  = _buffer;
    _writePtr = _buffer;
    _endPtr   = NULL;
    _wrapPtr  = NULL;

    return;
}

// ----------------------------------------------------------------------

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

size_t
CircularBuffer::reverse ( size_t offset )
{
    size_t  rpos = 0;  // reverse bytes possible
    size_t  wrap = 0;

    if ( _endPtr == NULL && _wrapPtr != NULL ) {
        rpos  = (_readPtr - _buffer);
        wrap  = (this->spaceAvailable() - (_buffsize - (_wrapPtr - _buffer)));

#       ifdef CB_DEBUG
        printf("CB::reverse() pos wrap front: %u, wrap: %u\n", 
               rpos, wrap);
#       endif

        rpos += wrap;
    } else if ( _endPtr == NULL ) {  // first time thru
        rpos = _readPtr - _buffer;
    } else {
        rpos = _readPtr - _writePtr;
    }


    if ( offset > rpos ) {
	offset = 0;
    } else if ( _endPtr == NULL && offset > (size_t)(_readPtr - _buffer) ) {
        wrap = offset - (_readPtr - _buffer);

#       ifdef CB_DEBUG
        printf("CB::reverse frontrev: %u unwrap rev: %u\n", 
            (_readPtr - _buffer), wrap);
#       endif

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

size_t
CircularBuffer::skip ( size_t offset )
{
    size_t  sz  = this->dataAvailable();

    if ( offset > this->fullDataAvailable() ) {
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

size_t
CircularBuffer::dataAvailable()
{
    size_t  size = 0;

    if ( _endPtr == NULL )
        size = _writePtr - _readPtr;
    else
        size = _endPtr - _readPtr;

    return size;
}

size_t
CircularBuffer::fullDataAvailable()
{
    size_t   size = 0;

    if ( _endPtr == NULL )
	size = _writePtr - _readPtr;
    else
	size = (_endPtr - _readPtr) + (_writePtr - _buffer);

    return size;
}

// ----------------------------------------------------------------------

size_t
CircularBuffer::spaceAvailable()
{
    size_t  size = 0;

    if ( _endPtr == NULL )
        size = _buffsize - (_writePtr - _buffer);
    else
        size = _readPtr - _writePtr;

    return size;
}

size_t
CircularBuffer::fullSpaceAvailable()
{
    size_t  size = 0;

    if ( _endPtr == NULL )
        size = (_buffsize - (_writePtr - _buffer)) + (_readPtr - _buffer);
    else 
        size = _readPtr - _writePtr;

    return size;
}

// ----------------------------------------------------------------------

bool
CircularBuffer::isWrapped()
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

bool
CircularBuffer::resize ( size_t buffsize ) throw ( BufferException )
{
    if ( buffsize < MIN_CIRBUFFER_SIZE || buffsize > MAX_CIRBUFFER_SIZE) 
        return false;

    if ( buffsize < _buffsize ) {  // buffer size reduction
        if ( this->fullDataAvailable() > buffsize )
            return false;
 
        if ( this->isWrapped() ) { // realign buffer data in wrapped state
            size_t mvamt = _endPtr - _readPtr;
            char * start = _buffer + (buffsize - mvamt);

            ::memmove(start, _readPtr, mvamt);

            _readPtr = start;
            _endPtr  = start + mvamt;

        } else if ( _readPtr != _buffer ) {  // realign buffer if neccessary

            if ( ((size_t)(_writePtr - _buffer)) > buffsize ) {
                size_t mvamt = this->dataAvailable();
                
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
        throw BufferException("Error in realloc of buffer");

    // remap ptrs
    _readPtr  = _buffer + roffset;
    _writePtr = _buffer + woffset;
    _wrapPtr  = NULL;
    _endPtr   = NULL;

    if ( wrap )
        _endPtr = _buffer + eoffset;
 
    // shift data block to end
    if ( buffsize > _buffsize && wrap ) {
        size_t mvamt = this->dataAvailable();
        char * start = (_buffer + buffsize) - mvamt;

        ::memmove(start, _readPtr, mvamt);

        _readPtr = start;
        _endPtr  = start + mvamt;
    }

    _buffsize = buffsize;

    return true;
}

// ----------------------------------------------------------------------

char*
CircularBuffer::getWritePtr ( size_t * size )
{
    char   *ptr    = NULL;
    size_t  offset = 0;

    if ( size == NULL )
        return ptr;

    offset = this->spaceAvailable();

#   ifdef CB_DEBUG
    printf("CB::getWritePtr() space: %d size: %d\n", offset, *size);
#   endif
    
    if ( _endPtr == NULL && offset < *size ) {
        offset = _readPtr - _buffer;

        if ( offset < *size ) {
            _errstr = "CB::getWritePtr() Insufficient space";
            *size   = 0;
            return NULL;
        }

#       ifdef CB_DEBUG
        printf(" getWritePtr() WRITE WRAP\n");
#       endif
        
        _endPtr   = _writePtr;
        _writePtr = _buffer;
        _wrapPtr  = NULL;

    } else if ( (size_t) offset < *size ) {
	_errstr = "CB::getWritePtr() Insufficient space";
	*size   = 0;
	return NULL;
    }

    ptr   = _writePtr;
    *size = offset;

#   ifdef CB_DEBUG
    if ( _writePtr == _buffer )
        printf(" CB writing at start of buffer\n");
#   endif

    return ptr;
}

// ----------------------------------------------------------------------

void
CircularBuffer::setWritePtr ( size_t offset ) throw ( BufferException )
{
    if ( offset > this->spaceAvailable() )
        throw BufferException("Buffer overwrite attempted");

    _writePtr += offset;
    
#   ifdef CB_DEBUG
    printf("CB::setWritePtr() offset of %u\n", offset);
#   endif

    if ( (size_t)(_writePtr - _buffer) == _buffsize ) {
#       ifdef CB_DEBUG
        printf("  setWritePtr() WRITE WRAP\n");
#       endif

        _endPtr   = _writePtr;
        _writePtr = _buffer;
        _wrapPtr  = NULL;
    }

    return;
}

// ----------------------------------------------------------------------

char*
CircularBuffer::getReadPtr ( size_t * size )
{
    char    *ptr = NULL;
    size_t   rd;

    if ( size == NULL )
        return ptr;
    
    rd    = this->dataAvailable();
    *size = rd;

    if ( rd == 0 && _endPtr != NULL ) {
#       ifdef CB_DEBUG
        printf("CB::setReadPtr() READ WRAP\n");
#       endif
        
        _readPtr = _buffer;
        _wrapPtr = _endPtr;
        _endPtr  = NULL;

        rd    = this->dataAvailable();
        *size = rd;
    }
        
    if ( rd > 0 )
        ptr = _readPtr;

#   ifdef CB_DEBUG
    if ( _readPtr == _buffer )
	printf("CB: reading at start of buffer\n");
    printf("CB::getReadPtr() %u\n", rd);
#   endif

    return ptr;
}

// ----------------------------------------------------------------------

void
CircularBuffer::setReadPtr ( size_t offset ) throw ( BufferException )
{
    if ( offset > this->dataAvailable() )
        throw BufferException("Buffer overread attempted!");

    _readPtr += offset;
    
#   ifdef CB_DEBUG
    printf("CB::setReadPtr() offset of %d\n", offset);
#   endif
    
    if ( _endPtr != NULL && _readPtr == _endPtr ) {
#       ifdef CB_DEBUG
        printf("CB::setReadPtr() READ WRAP\n");
#       endif

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
