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
 *  does not change any vals to network order (big endian), but if such
 *  behavior is needed, it would be easily provided here.
**/


int
Packer::Pack ( char * buffer, size_t buflen,
               const char * val,  size_t val_len )
{
    int pk = Packer::Pack(buffer, buflen, val_len);

    if ( pk <= 0 || buflen < pk + val_len )
        return -1;

    ::memcpy(buffer + pk, val, val_len);

    pk  += val_len;
    pk  += Packer::Pad(buffer, (buflen - pk), pk);

    return pk;
}

int
Packer::Pack ( char * buffer, size_t buflen, const std::string & val )
{
    return Packer::Pack(buffer, buflen, val.c_str(), val.length());
}


int
Packer::Pack ( char * buffer, size_t buflen, int8_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(int8_t*)buffer = val;

    return sizeof(val);
}

int
Packer::Pack( char * buffer, size_t buflen, uint8_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(uint8_t*)buffer = val;

    return sizeof(val);
}

int
Packer::Pack( char * buffer, size_t buflen, int16_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(int16_t*)buffer = val;

    return sizeof(val);
}

int
Packer::Pack( char * buffer, size_t buflen, uint16_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(uint16_t*)buffer = val;

    return sizeof(val);
}

int
Packer::Pack( char * buffer, size_t buflen, int32_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(int32_t*)buffer = val;

    return sizeof(val);
}

int
Packer::Pack( char * buffer, size_t buflen, uint32_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(uint32_t*)buffer = val;

    return sizeof(val);
}


int
Packer::Pack( char * buffer, size_t buflen, int64_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(int64_t*)buffer = val;

    return sizeof(val);
}

int
Packer::Pack( char * buffer, size_t buflen, uint64_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(uint64_t*)buffer = val;

    return sizeof(val);
}


//---------------------------------------------------------------------


int
Packer::Unpack ( const char * buffer, size_t  buflen,
                 char       * val,    size_t  vallen,
                 size_t & val_written )
{
    size_t skip = 0;
    int    upk  = Packer::Unpack(buffer, buflen, val_written);

    if ( vallen < val_written )
        return -1;
    else if ( buflen < upk + val_written )
        return -1;

    ::memcpy(val, buffer + upk, val_written);

    skip = val_written % PACKER_WORDSIZE;

    return(upk + val_written + skip);
}

int
Packer::Unpack ( const char * buffer, size_t  buflen, std::string & val )
{
    uint32_t  len  = 0;
    size_t    skip = 0;

    int  upk = Packer::Unpack(buffer, buflen, len);

    if ( upk < 0 || buflen < (upk + len) )
        return -1;

    val.assign(buffer + upk, len);
    skip = Packer::Skip(len);

    return (upk + len + skip);
}

int
Packer::Unpack ( const char * buffer, size_t buflen, int8_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(int8_t*)buffer;

    return sizeof(val);
}

int
Packer::Unpack ( const char * buffer, size_t buflen, uint8_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(uint8_t*)buffer;

    return sizeof(val);
}

int
Packer::Unpack ( const char * buffer, size_t  buflen, int16_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(int16_t*)buffer;

    return sizeof(val);
}

int
Packer::Unpack ( const char * buffer, size_t  buflen, uint16_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(uint16_t*)buffer;

    return sizeof(val);
}

int
Packer::Unpack ( const char * buffer, size_t  buflen, int32_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(int32_t*)buffer;

    return sizeof(val);
}

int
Packer::Unpack ( const char * buffer, size_t  buflen, uint32_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(uint32_t*)buffer;

    return sizeof(val);
}

int
Packer::Unpack ( const char * buffer, size_t  buflen, int64_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(int64_t*)buffer;

    return sizeof(val);
}

int
Packer::Unpack ( const char * buffer, size_t  buflen, uint64_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(uint64_t*)buffer;

    return sizeof(val);
}


} // namespace

// _TNMSCORE_PACK_CPP_

