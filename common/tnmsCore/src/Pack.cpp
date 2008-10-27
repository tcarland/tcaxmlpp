#define _TNMSCORE_PACK_CPP_


#include "Pack.h"


namespace tnmsCore {


//---------------------------------------------------------------------
/** The Packer Class provides a common interface in the overridden Pack() 
 *  function for ensure proper type encoding within our transmit buffers.
 *  This is purely for adherence to the tnmsProtocol spec. that encodes
 *  a length field prior to any variable length fields. 
 *  The Packer is not meant to handle the encryption and/or compression of 
 *  a given buffer, but possibly network order of byte fields where 
 *  appropriate. The current implementation assumes little endian and 
 *  does not change any values to network order (big endian), but if such
 *  behavior is needed, it would be easily provided here.
**/


int
Packer::Pack ( char * buffer, size_t buffer_len,
               const char * value,  size_t val_len )
{
    int pk = Packer::Pack(buffer, buffer_len, val_len);

    if ( pk <= 0 || buffer_len < pk + val_len )
        return -1;

    ::memcpy(buffer + pk, value, val_len);

    pk  += val_len;
    pk  += Packer::Pad(buffer, (buffer_len - pk), pk);

    return pk;
}

int
Packer::Pack ( char * buffer, size_t buffer_len, const std::string & value )
{
    return Packer::Pack(buffer, buffer_len, value.c_str(), value.length());
}


int
Packer::Pack ( char * buffer, size_t buffer_len, int8_t value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    *(int8_t*)buffer = value;

    return sizeof(value);
}

int
Packer::Pack( char * buffer, size_t buffer_len, uint8_t value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    *(uint8_t*)buffer = value;

    return sizeof(value);
}

int
Packer::Pack( char * buffer, size_t buffer_len, int16_t value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    *(int16_t*)buffer = value;

    return sizeof(value);
}

int
Packer::Pack( char * buffer, size_t buffer_len, uint16_t value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    *(uint16_t*)buffer = value;

    return sizeof(value);
}

int
Packer::Pack( char * buffer, size_t buffer_len, int32_t value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    *(int32_t*)buffer = value;

    return sizeof(value);
}

int
Packer::Pack( char * buffer, size_t buffer_len, uint32_t value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    *(uint32_t*)buffer = value;

    return sizeof(value);
}


int
Packer::Pack( char * buffer, size_t buffer_len, int64_t value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    *(int64_t*)buffer = value;

    return sizeof(value);
}

int
Packer::Pack( char * buffer, size_t buffer_len, uint64_t value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    *(uint64_t*)buffer = value;

    return sizeof(value);
}


//---------------------------------------------------------------------


int
Packer::Unpack ( char   * buffer, size_t  buffer_len,
                 char   * value,  size_t  val_len,
                 size_t & val_written )
{
    size_t skip = 0;
    int    upk  = Packer::Unpack(buffer, buffer_len, val_written);

    if ( val_len < val_written )
        return -1;
    else if ( buffer_len < upk + val_written )
        return -1;

    ::memcpy(value, buffer + upk, val_written);

    skip = val_written % PACKER_WORDSIZE;

    return(upk + val_written + skip);
}

int
Packer::Unpack ( char * buffer, size_t  buffer_len, std::string & value )
{
    uint32_t  len  = 0;
    size_t    skip = 0;

    int  upk = Packer::Unpack(buffer, buffer_len, len);

    if ( upk < 0 || buffer_len < (upk + len) )
        return -1;

    value.assign(buffer + upk, len);
    skip = Packer::Skip(len);

    return (upk + len + skip);
}

int
Packer::Unpack ( char * buffer, size_t buffer_len, int8_t & value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    value = *(int8_t*)buffer;

    return sizeof(value);
}

int
Packer::Unpack ( char * buffer, size_t buffer_len, uint8_t & value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    value = *(uint8_t*)buffer;

    return sizeof(value);
}

int
Packer::Unpack ( char   * buffer, size_t  buffer_len, int16_t & value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    value = *(int16_t*)buffer;

    return sizeof(value);
}

int
Packer::Unpack ( char   * buffer, size_t  buffer_len, uint16_t & value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    value = *(uint16_t*)buffer;

    return sizeof(value);
}

int
Packer::Unpack ( char   * buffer, size_t  buffer_len, int32_t & value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    value = *(int32_t*)buffer;

    return sizeof(value);
}

int
Packer::Unpack ( char   * buffer, size_t  buffer_len, uint32_t & value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    value = *(uint32_t*)buffer;

    return sizeof(value);
}

int
Packer::Unpack ( char   * buffer, size_t  buffer_len, int64_t & value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    value = *(int64_t*)buffer;

    return sizeof(value);
}

int
Packer::Unpack ( char   * buffer, size_t  buffer_len, uint64_t & value )
{
    if ( buffer_len < sizeof(value) )
        return -1;

    value = *(uint64_t*)buffer;

    return sizeof(value);
}


} // namespace

// _TNMSCORE_PACK_CPP_

