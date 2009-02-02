#ifndef _TNMSCORE_PACK_H_
#define _TNMSCORE_PACK_H_

#include <string>
#include <inttypes.h>


namespace tnmsCore {


#define PACKER_WORDSIZE 4


class Packer {

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
        size_t  pad = wtlen & PACKER_WORDSIZE;
        return (pad == 0) ? 0 : PACKER_WORDSIZE - pad;
    }

    //----------------------------------------------------------------------

    static inline size_t Skip ( const size_t & length )
    {
        return Packer::PadLen(length);
    }

    //----------------------------------------------------------------------

    static inline size_t Pad ( char * buffer, size_t  buflen,
                               const size_t & wtlen )
    {
        size_t  pad = Packer::PadLen(wtlen);

        if ( pad > buflen )
            return buflen;

        return pad;
    }

};


}  // namespace


#endif  //  _TNMSCORE_PACK_H_

