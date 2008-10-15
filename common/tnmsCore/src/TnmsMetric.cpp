/*
 * TnmsMetric.cpp
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */
#define _TNMSMETRIC_CPP_


#include "TnmsMetric.h"
#include "Pack.hpp"


namespace tnmscore {



TnmsMetric::TnmsMetric()
    : TnmsMessage(RECORD_METRIC)
{}

TnmsMetric::TnmsMetric ( const std::string & name, 
                         const TnmsOid     & oid )
    : TnmsMessage(name, oid, RECORD_METRIC)
{}


std::string
TnmsMetric::getValue()
{
    return _valueStr;
}


template<typename T>
T
TnmsMetric::getValue()
{
    if ( _valType == TNMS_STRING )
        return 0;

    T  val = static_cast<T>(_value);

    return val;
}


int
TnmsMetric::getValueType()
{
    return((int)this->_valueType);
}


bool
TnmsMetric::setValue ( eValueType valtype, const std::string & value )
{
    _valType  = TNMS_STRING;
    _valueStr = value;
    return true;
}


template<typename T>
bool
TnmsMetric::setValue ( eValueType valtype, T  & value )
{
    if ( valtype > TNMS_NONE && valtype < TNMS_STRING ) {
        _valType = valtype;
        _value   = static_cast<uint64_t>(value);
        return true;
    }

    return false;
}


ssize_t
TnmsMetric::serialize ( char * buffer, size_t  buffer_len )
{
    ssize_t   pk, wt = 0;

    char * wptr = buffer;

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


ssize_t
TnmsMetric::deserialize ( char * buffer, size_t  buffer_len )
{
    char     * rptr;
    size_t     rsz, rd = 0;
    ssize_t    upk;

    if ( buffer_len < this->size() )
        return -1;

    rptr  = buffer;
    rsz   = buffer_len;

    upk   = this->_element_oid.deserialize(rptr, (rsz-rd));
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = Packer::unpack(rptr, (rsz-rd), _element_name);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    uint16_t  value_type;
    upk   = Packer::unpack(rptr, (rsz-rd), value_type);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;
    this->_valType = value_type;

    uint32_t  vallen = 0;
    if ( _valType == TNMS_STRING ) {
        upk = Packer::unpack(rptr, (rsz-rd), _valueStr);
        if ( upk < 0 )
            return -1;
        rd   += upk;
        rptr += upk;
    } else if ( _valType > 0 ) {
        upk = Packer::unpack(rptr, (rsz-rd), vallen);
        if ( upk < 0 )
            return -1;
        rd   += upk;
        rptr += upk;

        if ( vallen == sizeof(uint64_t) )
            upk = Packer::unpack(rptr, (rsz-rd), _value);
        else
            upk = vallen;

        if ( upk < 0 )
            return -1;
        rd   += upk;
        rptr += upk;
    }

    upk   = Packer::unpack(rptr, (rsz-rd), _pvt);
    if ( upk < 0 )
        return -1;
    rd   += upk;

    return rd;
}


size_t
TnmsMetric::size() const
{
    size_t  sz  = 0;

    sz   = _element_name.length() +_element_oid.size() 
        + _valueStr.size() + _pvt.size();
    sz  += ( 4 * sizeof(uint32_t)) + sizeof(uint64_t);

    return sz;
}


int
TnmsMetric::message_type() const
{
    return _message_type;
}


} // namespace

// _TNMSMETRIC_CPP_

