/*
 * TnmsMetric.cpp
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */
#define _TNMSCORE_TNMSMETRIC_CPP_


#include "TnmsMetric.h"
#include "Pack.h"


namespace tnmsCore {



TnmsMetric::TnmsMetric()
    : TnmsMessage(METRIC_MESSAGE)
{}

TnmsMetric::TnmsMetric ( const std::string & name,
                         const TnmsOid     & oid )
    : TnmsMessage(name, oid, METRIC_MESSAGE)
{}

TnmsMetric::~TnmsMetric() {}


const std::string&
TnmsMetric::getValue() const
{
    return _valueStr;
}


template<typename T>
const T&
TnmsMetric::getValue() const
{
    if ( _valType == TNMS_STRING )
        return 0;

    T  val = static_cast<T>(_value);

    return val;
}


int
TnmsMetric::getValueType() const
{
    return((int)this->_valType);
}


bool
TnmsMetric::setValue ( eValueTypes valtype, const std::string & value )
{
    _valType  = TNMS_STRING;
    _valueStr = value;
    return true;
}


template<typename T>
bool
TnmsMetric::setValue ( eValueTypes valtype, T  & value )
{
    if ( valtype > TNMS_NONE && valtype < TNMS_STRING ) {
        _valType = valtype;
        _value   = static_cast<uint64_t>(value);
        return true;
    }

    return false;
}


const std::string&
TnmsMetric::getPvtData() const
{
    return _pvt;
}

bool
TnmsMetric::setPvtData ( const std::string & data )
{
    _pvt = data;
    return true;
}


ssize_t
TnmsMetric::serialize ( char * buffer, size_t  buffer_len )
{
    char    * wptr;
    size_t    wsz;
    ssize_t   pk, wt = 0;

    wptr = buffer;
    wsz  = buffer_len;

    pk    = _element_oid.serialize(wptr, (wsz-wt));
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = Packer::Pack(wptr, (wsz-wt), _element_name);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    uint16_t  type = _valType;
    pk  = Packer::Pack(wptr, (wsz-wt), type);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    if ( _valType == TNMS_STRING )
        pk = Packer::Pack(wptr, (wsz-wt), _valueStr);
    else
        pk = Packer::Pack(wptr, (wsz-wt), _value);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    // TO-DO: pvt data
    std::string  pvt;

    pk   = Packer::Pack(wptr, (wsz-wt), pvt);
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

    upk   = Packer::Unpack(rptr, (rsz-rd), _element_name);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    uint16_t  value_type;
    upk   = Packer::Unpack(rptr, (rsz-rd), value_type);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;
    this->_valType = (eValueTypes) value_type;

    uint32_t  vallen = 0;
    if ( _valType == TNMS_STRING ) {
        upk = Packer::Unpack(rptr, (rsz-rd), _valueStr);
        if ( upk < 0 )
            return -1;
        rd   += upk;
        rptr += upk;
    } else if ( _valType > 0 ) {
        upk = Packer::Unpack(rptr, (rsz-rd), vallen);
        if ( upk < 0 )
            return -1;
        rd   += upk;
        rptr += upk;

        if ( vallen == sizeof(uint64_t) )
            upk = Packer::Unpack(rptr, (rsz-rd), _value);
        else
            upk = vallen;

        if ( upk < 0 )
            return -1;
        rd   += upk;
        rptr += upk;
    }

    upk   = Packer::Unpack(rptr, (rsz-rd), _pvt);
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


} // namespace

// _TNMSMETRIC_CPP_

