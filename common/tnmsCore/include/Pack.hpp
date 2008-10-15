#ifndef _TNMSCORE_SERIALIZE_HPP_
#define _TNMSCORE_SERIALIZE_HPP_

#include <string>

#include "tcanetpp_types.h"  // tcanetpp


namespace tnmsCore {


#define PACKER_WORDSIZE 4


class Packer {

public:

    static int Pack ( char       * buffer,
                      size_t       buffer_len,
                      const char * value,
                      size_t       val_len )
    {
        int pk = Packer::Pack<uint32_t>(buffer, buffer_len, val_len);

        if ( pk <= 0 || buffer_len < pk + val_len )
            return -1;

        ::memcpy(buffer + pk, value, val_len);

        pk  += val_len;
        pk  += Packer::Pad(buffer, (buffer_len - pk), pk);

        return pk;
    }


    static int Pack ( char              * buffer,
                      size_t              buffer_len,
                      const std::string & value,
                      size_t              val_len )
    {
        return Packer::Pack(buffer, buffer_len, value.c_str(), val_len);
    }


    template<typename T>
    static int Pack ( char * buffer, size_t buffer_len, T & value )
    {
        if ( buffer_len < sizeof(value) )
            return -1;

        *(T*)buffer = value;

        return sizeof(value);
    }


    //------------------------------------------------------------------------


    static int Unpack ( char   * buffer,
                        size_t   buffer_len,
                        char   * value,
                        size_t   val_len,
                        size_t & val_written )
    {
        size_t skip = 0;
        int    upk  = Packer::Unpack<uint32_t>(buffer, buffer_len, val_written);

        if ( val_len < val_written )
            return -1;
        else if ( buffer_len < upk + val_written )
            return -1;

        ::memcpy(value, buffer + upk, val_written);

        skip = val_written % PACKER_WORDSIZE;

        return(upk + val_written + skip);
    }


    static int Unpack ( char        * buffer,
                        size_t        buffer_len,
                        std::string & value,
                        size_t      & val_written )
    {

        size_t  len = 0, skip = 0;

        int  upk = Packer::Unpack<uint32_t>(buffer, buffer_len, (uint32_t&) len);

        if ( upk < 0 || buffer_len < (upk + len) )
            return -1;

        value.assign(buffer + upk, len);
        skip = Packer::Skip(len);
        val_written = value.length();

        return (upk + len + skip);
    }


    template<typename T>
    static int Unpack ( char * buffer, size_t buffer_len, T & value )
    {
        if ( buffer_len < sizeof(value) )
            return -1;

        value = *(T*)buffer;

        return sizeof(value);
    }

    //------------------------------------------------------------------------


    static inline size_t PadLen ( const size_t & wtlen )
    {
        size_t  pad = wtlen & PACKER_WORDSIZE;
        return (pad == 0) ? 0 : PACKER_WORDSIZE - pad;
    }



    static inline size_t Skip ( const size_t & length )
    {
        return Packer::PadLen(length);
    }


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

#endif  // _TNMSCORE_SERIALIZE_HPP_
