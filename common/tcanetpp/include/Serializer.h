#ifndef _TCANETPP_SERIALIZER_H_
#define _TCANETPP_SERIALIZER_H_

extern "C" {
# include <string.h>
}
#include <string>
#include <inttypes.h>


namespace tcanetpp {


#define SERIALIZER_WORDSIZE 4


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

