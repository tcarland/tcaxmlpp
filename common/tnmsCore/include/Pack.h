#ifndef _TNMSCORE_PACK_H_
#define _TNMSCORE_PACK_H_

#include <string>
#include <inttypes.h>


namespace tnmsCore {


#define PACKER_WORDSIZE 4


class Packer {

  public:

    static int  Pack   ( char * buffer, size_t buffer_len, 
                         const char * value, size_t value_len );

    static int  Pack   ( char * buffer, size_t buffer_len,
                         const std::string & value );

    static int  Pack   ( char * buffer, size_t buffer_len, int8_t   value );
    static int  Pack   ( char * buffer, size_t buffer_len, uint8_t  value );
    static int  Pack   ( char * buffer, size_t buffer_len, int16_t  value );
    static int  Pack   ( char * buffer, size_t buffer_len, uint16_t value );
    static int  Pack   ( char * buffer, size_t buffer_len, int32_t  value );
    static int  Pack   ( char * buffer, size_t buffer_len, uint32_t value );
    static int  Pack   ( char * buffer, size_t buffer_len, int64_t  value );
    static int  Pack   ( char * buffer, size_t buffer_len, uint64_t value );

    //---------------------------------------------------------------------

    static int  Unpack ( char * buffer, size_t buffer_len, 
                         char * value,  size_t value_len, 
                         size_t  & val_written );

    static int  Unpack ( char * buffer, size_t buffer_len,
                         std::string & value );

    static int  Unpack ( char * buffer, size_t buffer_len, int8_t   & value );
    static int  Unpack ( char * buffer, size_t buffer_len, uint8_t  & value );
    static int  Unpack ( char * buffer, size_t buffer_len, int16_t  & value );
    static int  Unpack ( char * buffer, size_t buffer_len, uint16_t & value );
    static int  Unpack ( char * buffer, size_t buffer_len, int32_t  & value );
    static int  Unpack ( char * buffer, size_t buffer_len, uint32_t & value );
    static int  Unpack ( char * buffer, size_t buffer_len, int64_t  & value );
    static int  Unpack ( char * buffer, size_t buffer_len, uint64_t & value );

    //---------------------------------------------------------------------
    
    static inline size_t PadLen ( const size_t & wtlen )
    {
        size_t  pad = wtlen & PACKER_WORDSIZE;
        return (pad == 0) ? 0 : PACKER_WORDSIZE - pad;
    }

    //----------------------------------------------------------------------

    static inline size_t Skip ( const size_t & length )
    {
        return Packer::PadLen(length);
    }

    //----------------------------------------------------------------------

    static inline size_t Pad ( char * buffer, size_t  buffer_len,
                               const size_t & wtlen )
    {
        size_t  pad = Packer::PadLen(wtlen);

        if ( pad > buffer_len )
            return buffer_len;

        return pad;
    }

};


}  // namespace


#endif  //  _TNMSCORE_PACK_H_

