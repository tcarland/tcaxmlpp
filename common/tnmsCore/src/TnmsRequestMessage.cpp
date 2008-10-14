#define _TNMSCORE_TNMSREQUESTMESSAGE_CPP_

#include "TnmsRequestMessage.h"


namespace tnmscore {


TnmsRequestMessage::TnmsRequestMessage()
    : _message_type(RECORD_NONE)
{}

TnmsRequestMessage::TnmsRequestMessage ( const std::string & name ) 
    : _element_name(name),
      _message_type(RECORD_REQUEST)
{}

TnmsRequestMessage::TnmsRequestMessage ( const TnmsOid & oid )
    : _element_oid(oid),
      _message_type(RECORD_REQUEST)
{}

TnmsRequestMessage::TnmsRequestMessage ( const std::string & name,
                                         const TnmsOid     & oid )
    : _element_name(name),
      _element_oid(oid),
      _message_type(RECORD_REQUEST)
{}

TnmsRequestMessage::~TnmsRequestMessage() {}


ssize_t
TnmsRequestMessage::serialize( char * buffer, size_t buffer_len ) 
{
    ssize_t  pk, wt = 0;

    char * wptr = buffer;

    pk    = Packer::pack(wptr, (buffer_len - wt), _message_type);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = Packer::pack(wptr, (buffer_len - wt), _oid->size());
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = _oid->serialize(wptr, (buffer_len - wt));
    if ( pk < 0 )
        return -1;;
    wt   += pk;
    wptr += pk;

    pk    = Packer::pack(wptr (buffer_len - wt), _element_name);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    
    return wt;
}


size_t
TnmsRequestMessage::size() const 
{
    size_t sz = 0;

    sz  = _element_name.length() + _oid.size();
    sz += 2 * sizeof(uint32_t);

    return sz;
}


// _TNMSCORE_TNMSREQUESTMESSAGE_CPP_
