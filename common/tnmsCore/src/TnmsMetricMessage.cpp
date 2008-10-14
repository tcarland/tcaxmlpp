/*
 * TnmsMetricMessage.cpp
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */
#define _TNMSMETRICMESSAGE_CPP_


#include "TnmsMetricMessage.h"
#include "Pack.hpp"


namespace tnmscore {



TnmsMetricMessage::TnmsMetricMessage()
    : TnmsMessage(RECORD_METRIC)
{}

TnmsMetricMessage::TnmsMetricMessage ( const std::string & name, 
                                       const TnmsOid     & oid )
    : TnmsMessage(name, oid, RECORD_METRIC)
{}


std::string
TnmsMetricMessage::getValue()
{
    return _valueStr;
}


template<typename T>
T
TnmsMetricMessage::getValue()
{
    if ( _valType == TNMS_STRING )
        return 0;

    T  val = static_cast<T>(_value);

    return val;
}


int
TnmsMetricMessage::getValueType()
{
    return((int)this->_valueType);
}


bool
TnmsMetricMessage::setValue ( eValueType valtype, const std::string & value )
{
    _valType  = TNMS_STRING;
    _valueStr = value;
    return true;
}


template<typename T>
bool
TnmsMetricMessage::setValue ( eValueType valtype, T  & value )
{
    if ( valtype > TNMS_NONE && valtype < TNMS_STRING ) {
        _valType = valtype;
        _value   = static_cast<uint64_t>(value);
        return true;
    }

    return false;
}


ssize_t
TnmsMetricMessage::serialize ( char * buffer, size_t  buffer_len )
{
    ssize_t   pk, wt = 0;

    char * wptr = buffer;

    pk    = Packer::pack(wptr, (buffer_len - wt), (uint32_t) _oid->size());
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = _oid->serialize(wptr, (buffer_len - wt));
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = Packer::pack(wptr, (buffer_len - wt), _element_name);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk  = Packer::pack(wptr, (buffer_len - wt), (uint16_t) _valueType);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    if ( _valueType == TNMS_STRING )
        pk = Packer::pack(wptr, (buffer_len - wt), _valueStr);
    else
        pk = Packer::pack(wptr, (buffer_len - wt), _value);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    // TO-DO: pvt data
    std::string  pvt;

    pk   = Packer::pack(wptr, (size - wt), pvt);
    if ( pk < 0 )
        return -1;

    wt += pk;

    return wt;
}


size_t
TnmsMetricMessage::size() const
{
    size_t  sz  = 0;

    sz   = _element_name.length() +_element_oid.size() 
        + _valueStr.size() + _pvt.size();
    sz  += ( 4 * sizeof(uint32_t)) + sizeof(uint64_t);

    return sz;
}


int
TnmsMetricMessage::message_type() const
{
    return _message_type;
}


} // namespace

// _TNMSMETRICMESSAGE_CPP_

