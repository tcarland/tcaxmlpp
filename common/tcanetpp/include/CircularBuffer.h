/**
  *  A circular buffer implementation.
  *
  *   @Author  tcarland@gmail.com
  *   @Version 5.0
 **/
#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_


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
#define MINBUFFSIZE        1
#define MAXBUFFSIZE	   2^31
#define DEFAULT_BUFFSIZE   256000
#define BUFFER_MULTIPLIER  1000


// ----------------------------------------------------------------------

/**  Buffer Exception class
  *   Thrown in 'set' methods if buffer overwrite or overread
  *   is attempted.
 **/
class BufferException : public Exception {
  public:
    BufferException ( const std::string & s_err ) : Exception(s_err) {}
};

// ----------------------------------------------------------------------

class CircularBuffer {

  public:

    /**  Class Constructor for a buffer instance. 
      *  Throws an exception if the malloc fails or the total
      *  buffer size is out of range. If no count is given,
      *  a default is used.
     **/
    CircularBuffer() throw ( BufferException );
    CircularBuffer ( size_t totalsize ) throw ( BufferException );

    /**  Performs a full copy of the buffer including the current
      *  read/write/wrap state.
     **/
    CircularBuffer ( const CircularBuffer & buffer ) throw ( BufferException );

    virtual ~CircularBuffer();


    /**   Reads <n> bytes from the circular buffer to the
      *   provided buffer, returning the number of bytes
      *   actually copied to the buffer.
      *
      *   @param buff  the buffer to fill.
      *   @param n     the number of bytes to copy. 
     **/
    virtual size_t read ( void * buff, size_t n );


    /**   Writes <n> bytes from the provided buffer to the 
      *   CirBuffer returning the number of bytes written.
      *
      *   @param buff  the buffer from which to copy.
      *   @param n     the number of bytes to copy.
     **/
    virtual size_t write ( const void * buff, size_t n );


    /**   Returns the number of bytes available for reading
      *   contiguously (without a buffer wrap).
     **/
    size_t         dataAvailable();
    
    /**   Returns the full amount of data available including
      *   wrapped data.
     **/
    size_t         fullDataAvailable();


    /**   The amount of contigous space available for writing
      *   without a buffer wrap.
     **/
    size_t         spaceAvailable();

    /**   The total amount space available for writing including
      *   any 'wrapped' space.
     **/
    size_t         fullSpaceAvailable();
    

    /**   Returns a boolean indicating the current buffer state */
    bool           isWrapped();
    

    /**   Reverses the buffer by the provided number of bytes.
      *   NOTE that this is only a safe operation immediatly
      *   following a read and while NO writes have occured.
     **/
    size_t         reverse ( size_t offset );

    /**   Skips the read position forward the provided number of bytes */
    size_t         skip ( size_t offset );


    /**     Resizes the buffer to the requested size. This operation 
      *   may result in the need to 'realign' the data blocks in the 
      *   buffer to allow the resize to occur. The underlying effect 
      *   being a call to memmove and a realloc.
      *     The resize can fail if the size requested is larger than the 
      *   max allowed, or is smaller than an existing data block within 
      *   the buffer. The method can throw an exception if the call 
      *   to realloc fails.
     **/
    bool           resize ( size_t buffsize ) throw ( BufferException );


    /**@{
      *   Resets the buffer and all pointers back to zero.
     **/
    virtual void   clear();
    inline  void   reset() { return clear(); }
    /*@}*/
    
    inline size_t  size()              { return _buffsize; }



    /*  DISCLAIMER:
     *  The following methods are primarily used internally, however 
     *  direct use is allowed to provide for faster reads/writes by 
     *  eliminating an extra memcpy. The standard read/write() methods 
     *  above accept a user provided buffer to write to or read from
     *  by performing a memcpy for each operation. The following methods 
     *  allow access to the internal buffer pointers essentially providing 
     *  direct access to the underlying buffer. The methods are reasonably
     *  straightforward, but are only safe as long as the sizes provided 
     *  are accurate. The methods are definately not thread-safe.
     */

  public:

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
    char* getWritePtr ( size_t * size );


    /**   Accepts @param  offset as the size written to the buffer,
      *   and updates the writePtr to the new position (ie. the 
      *   last position + offset ).
      *
      *   The method will throw a BufferException if the offset
      *   provided is not valid (ie. would cause an overwrite)
     **/
    void  setWritePtr ( size_t offset ) throw ( BufferException );


    /**   Returns the readPtr to the first readable position if
      *   there is space to read, or returns NULL on no data. 
      *   
      *   @param size  is populated with the total size available
      *   for reading.
     **/
    char* getReadPtr ( size_t * size );


    /**   This method sets the read pointer to the new position or
      *   throws an exception if the offset provided would cause an overread.
      *   
      *   @param offset  is the size (bytes) read off the buffer.
     **/
    void  setReadPtr ( size_t offset ) throw ( BufferException );


  protected:

    void  init() throw ( BufferException );


  private:

    char*            _buffer;    
    char*            _readPtr;
    char*            _writePtr;
    char*            _endPtr;   
    char*            _wrapPtr;   

    size_t           _buffsize;
    std::string      _errstr;
};


}  // namespace



#endif //  _TCANETPP_CIRCULARBUFFER_H_
