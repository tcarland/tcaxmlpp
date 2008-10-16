#define _TNMSCORE_TNMSMESSAGE_CPP_

#include "TnmsMessage.h"
#include "Pack.hpp"


namespace tnmsCore {


TnmsMessage::TnmsMessage ( int type )
    : _message_type(type)
{}

TnmsMessage::TnmsMessage ( const std::string & name, int type )
    : _element_name(name),
      _message_type(type)
{}

TnmsMessage::TnmsMessage ( const TnmsOid & oid, int type )
    : _element_oid(oid),
      _message_type(type)
{}

TnmsMessage::TnmsMessage ( const std::string & name,
                           const TnmsOid     & oid,
                           int                 type )
    : _element_name(name),
      _element_oid(oid),
      _message_type(type)
{}


TnmsMessage::~TnmsMessage() {}


const std::string&
TnmsMessage::getElementName() const
{
    return this->_element_name;
}

const TnmsOid&
TnmsMessage::getElementOid() const
{
    return this->_element_oid;
}


ssize_t
TnmsMessage::serialize ( char * buffer, size_t buffer_len )
{
    ssize_t  pk, wt = 0;

    char * wptr = buffer;

    pk    = Packer::Pack(wptr, (buffer_len - wt), _message_type);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = _element_oid.serialize(wptr, (buffer_len - wt));
    if ( pk < 0 )
        return -1;;
    wt   += pk;
    wptr += pk;

    pk    = Packer::Pack(wptr, (buffer_len - wt), _element_name);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    
    return wt;

    return -1;
}


ssize_t
TnmsMessage::deserialize ( char * buffer, size_t buffer_len )
{
    char     * rptr;
    size_t     rsz, rd = 0;
    ssize_t    upk;

    if ( buffer_len < this->size() )
        return -1;

    rptr  = buffer;
    rsz   = buffer_len;

    upk   = Packer::Unpack(rptr, (rsz - rd), _message_type);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = this->_element_oid.deserialize(rptr, (rsz - rd));
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = Packer::Unpack(rptr, (rsz - rd), _element_name);
    if ( upk < 0 )
        return -1;
    rd   += upk;
   
    return rd;
}


size_t
TnmsMessage::size() const
{
    size_t sz = 0;

    if ( this->_message_type == NO_RECORD )
        return sz;

    sz  = sizeof(uint32_t) + _element_name.length() 
          + _element_oid.size();

    return sz;
}


uint16_t
TnmsMessage::message_type() const
{
    return this->_message_type;
}



}  // namespace

//  _TNMSCORE_TNMSMESSAGE_CPP_
