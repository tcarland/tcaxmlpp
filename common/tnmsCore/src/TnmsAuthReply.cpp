#define _TNMSCORE_TNMSAUTHREPLY_CPP_

#include "TnmsAuthReply.h"

#include "Serializer.h"
#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsCore {


eAuthType
TnmsAuthReply::authResult() const
{
    return _auth_result;
}

const std::string&
TnmsAuthReply::authReason() const
{
    return _auth_reason;
}

const std::string&
TnmsAuthReply::authData() const
{
    return _auth_data;
}


ssize_t
TnmsAuthReply::serialize ( char * buffer, size_t buffer_len )
{
    ssize_t  pk, wt = 0;
    char   * wptr   = buffer;

    if ( buffer_len < this->size() )
        return -1;

    pk    = Serializer::Pack(wptr, buffer_len - wt, _element_name);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = _element_oid.serialize(wptr, buffer_len - wt);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = Serializer::Pack(wptr, buffer_len - wt, ((uint16_t)_auth_result));
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = Serializer::Pack(wptr, buffer_len - wt, _auth_reason);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = Serializer::Pack(wptr, buffer_len - wt, _auth_data);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    LogFacility::Message   msg;
    msg << "TnmsAuthReply::serialize() result = " << this->_auth_result;
    LogFacility::LogMessage(msg);

    return wt;
}


ssize_t
TnmsAuthReply::deserialize ( const char * buffer, size_t buffer_len )
{
    const char * rptr;
    size_t       rsz, rd = 0;
    ssize_t      upk;

    if ( buffer_len < this->size() )
        return -1;

    rptr  = buffer;
    rsz   = buffer_len;

    upk   = Serializer::Unpack(rptr, (rsz-rd), _element_name);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = this->_element_oid.deserialize(rptr, (rsz-rd));
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    uint16_t result;
    upk   = Serializer::Unpack(rptr, (rsz-rd), result);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    _auth_result = (eAuthType) result;

    upk   = Serializer::Unpack(rptr, (rsz-rd), _auth_reason);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = Serializer::Unpack(rptr, (rsz-rd), _auth_data);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    LogFacility::Message   msg;
    msg << "TnmsAuthReply::deserialize() result = " << this->_auth_result;
    LogFacility::LogMessage(msg);

    return rd;
}


size_t
TnmsAuthReply::size() const
{
    size_t sz = 0;

    sz  = _element_name.length() + _element_oid.size() 
           + sizeof(uint16_t) + _auth_reason.length()
           + _auth_data.length();
    sz += (3 * sizeof(uint32_t)) + sizeof(uint16_t);

    return sz;
}



}  // namespace

// _TNMSCORE_TNMSAUTHREPLY_CPP_

