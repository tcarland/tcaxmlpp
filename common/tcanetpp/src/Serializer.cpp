/** 
  * @file Serializer.cpp
  *
 **/
#define _TCANETPP_SERIALIZER_CPP_

#include "Serializer.h"


namespace tcanetpp {


//---------------------------------------------------------------------

int
Serializer::Pack ( char * buffer, size_t buflen,
               const char * val,  size_t val_len )
{
    int pk = Serializer::Pack(buffer, (uint32_t)buflen, (uint32_t)val_len);

    if ( pk <= 0 || buflen < pk + val_len )
        return -1;

    ::memcpy(buffer + pk, val, val_len);

    pk  += val_len;
    pk  += Serializer::Pad(buffer, (buflen - pk), pk);

    return pk;
}

int
Serializer::Pack ( char * buffer, size_t buflen, const std::string & val )
{
    return Serializer::Pack(buffer, buflen, val.c_str(), val.length());
}


int
Serializer::Pack ( char * buffer, size_t buflen, int8_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(int8_t*)buffer = val;

    return sizeof(val);
}

int
Serializer::Pack( char * buffer, size_t buflen, uint8_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(uint8_t*)buffer = val;

    return sizeof(val);
}

int
Serializer::Pack( char * buffer, size_t buflen, int16_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(int16_t*)buffer = val;

    return sizeof(val);
}

int
Serializer::Pack( char * buffer, size_t buflen, uint16_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(uint16_t*)buffer = val;

    return sizeof(val);
}

int
Serializer::Pack( char * buffer, size_t buflen, int32_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(int32_t*)buffer = val;

    return sizeof(val);
}

int
Serializer::Pack( char * buffer, size_t buflen, uint32_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(uint32_t*)buffer = val;

    return sizeof(val);
}


int
Serializer::Pack( char * buffer, size_t buflen, int64_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(int64_t*)buffer = val;

    return sizeof(val);
}

int
Serializer::Pack( char * buffer, size_t buflen, uint64_t val )
{
    if ( buflen < sizeof(val) )
        return -1;

    *(uint64_t*)buffer = val;

    return sizeof(val);
}


//---------------------------------------------------------------------


int
Serializer::Unpack ( const char * buffer, size_t  buflen,
                 char       * val,    size_t  vallen,
                 size_t & val_written )
{
    size_t   skip = 0;
    uint32_t valw = 0;

    int  upk  = Serializer::Unpack(buffer, buflen, valw);

    val_written = valw;

    if ( vallen < val_written )
        return -1;
    else if ( buflen < upk + val_written )
        return -1;

    ::memcpy(val, buffer + upk, val_written);

    skip = val_written % SERIALIZER_WORDSIZE;

    return(upk + val_written + skip);
}

int
Serializer::Unpack ( const char * buffer, size_t  buflen, std::string & val )
{
    uint32_t  len  = 0;
    size_t    skip = 0;

    int  upk = Serializer::Unpack(buffer, buflen, len);

    if ( upk < 0 || buflen < (upk + len) )
        return -1;

    val.assign(buffer + upk, len);
    skip = Serializer::Skip(len);

    return (upk + len + skip);
}

int
Serializer::Unpack ( const char * buffer, size_t buflen, int8_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(int8_t*)buffer;

    return sizeof(val);
}

int
Serializer::Unpack ( const char * buffer, size_t buflen, uint8_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(uint8_t*)buffer;

    return sizeof(val);
}

int
Serializer::Unpack ( const char * buffer, size_t  buflen, int16_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(int16_t*)buffer;

    return sizeof(val);
}

int
Serializer::Unpack ( const char * buffer, size_t  buflen, uint16_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(uint16_t*)buffer;

    return sizeof(val);
}

int
Serializer::Unpack ( const char * buffer, size_t  buflen, int32_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(int32_t*)buffer;

    return sizeof(val);
}

int
Serializer::Unpack ( const char * buffer, size_t  buflen, uint32_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(uint32_t*)buffer;

    return sizeof(val);
}

int
Serializer::Unpack ( const char * buffer, size_t  buflen, int64_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(int64_t*)buffer;

    return sizeof(val);
}

int
Serializer::Unpack ( const char * buffer, size_t  buflen, uint64_t & val )
{
    if ( buflen < sizeof(val) )
        return -1;

    val = *(uint64_t*)buffer;

    return sizeof(val);
}


} // namespace

// _TCANETPP_SERIALIZER_CPP_

