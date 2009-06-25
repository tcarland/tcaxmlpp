/*
 * TnmsMetric.cpp
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */
#define _TNMSCORE_TNMSMETRIC_CPP_

#include "TnmsMetric.h"


#include "Serializer.h"
#include "StringUtils.h"
#include "LogFacility.h"
using namespace tcanetpp;



namespace tnmsCore {



TnmsMetric::TnmsMetric()
    : TnmsMessage(METRIC_MESSAGE)
{}

TnmsMetric::TnmsMetric ( const std::string & name, int message_type )
    : TnmsMessage(name, message_type)
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


int
TnmsMetric::getValueType() const
{
    return((int)this->_valType);
}


bool
TnmsMetric::setValue ( eValueType valtype, const std::string & value )
{
    _valType  = TNMS_STRING;
    _valueStr = value;
    return true;
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
TnmsMetric::serialize ( char * buffer, size_t  buffer_len ) const
{
    char    * wptr;
    size_t    wsz;
    ssize_t   pk, wt = 0;

    wptr = buffer;
    wsz  = buffer_len;

    // oidlen + oid
    pk    = _element_oid.serialize(wptr, (wsz-wt));
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    // element name
    pk    = Serializer::Pack(wptr, (wsz-wt), _element_name);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    // value type
    uint16_t  type = _valType;
    pk  = Serializer::Pack(wptr, (wsz-wt), type);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    // val len + value
    if ( _valType == TNMS_STRING ) {
        pk = Serializer::Pack(wptr, (wsz-wt), _valueStr);
        if ( pk < 0 )
            return -1;
        wt   += pk;
        wptr += pk;
    } else {
        uint32_t sz = (uint32_t) sizeof(_value);
        pk = Serializer::Pack(wptr, (wsz-wt), sz);
        if ( pk < 0 )
            return -1;
        wt   += pk;
        wptr += pk;

        pk = Serializer::Pack(wptr, (wsz-wt), _value);
        if ( pk < 0 )
            return -1;
        wt   += pk;
        wptr += pk;
    }

    pk   = Serializer::Pack(wptr, (wsz-wt), _pvt);
    if ( pk < 0 )
        return -1;
    wt += pk;

    LogFacility::LogMessage("TnmsMetric::serialize() " + StringUtils::toString<ssize_t>(wt));

    return wt;
}


ssize_t
TnmsMetric::deserialize ( const char * buffer, size_t  buffer_len )
{
    const char * rptr;
    ssize_t      upk;
    size_t       rsz, rd = 0;
    uint16_t     value_type = 0;
    uint32_t     vallen = 0;

    if ( buffer_len < this->size() )
        return -1;

    rptr  = buffer;
    rsz   = buffer_len;

    upk   = this->_element_oid.deserialize(rptr, (rsz-rd));
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = Serializer::Unpack(rptr, (rsz-rd), _element_name);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = Serializer::Unpack(rptr, (rsz-rd), value_type);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    this->_valType = (eValueType) value_type;

    if ( _valType == TNMS_STRING ) {
        upk = Serializer::Unpack(rptr, (rsz-rd), _valueStr);
        if ( upk < 0 )
            return -1;
        rd   += upk;
        rptr += upk;
    } else if ( _valType > 0 ) {
        upk = Serializer::Unpack(rptr, (rsz-rd), vallen);
        if ( upk < 0 )
            return -1;
        rd   += upk;
        rptr += upk;

        if ( vallen != sizeof(_value) )
            return -1;

        upk = Serializer::Unpack(rptr, (rsz-rd), _value);

        if ( upk < 0 )
            return -1;
        rd   += upk;
        rptr += upk;
    }

    upk   = Serializer::Unpack(rptr, (rsz-rd), _pvt);
    if ( upk < 0 )
        return -1;
    rd   += upk;

    LogFacility::LogMessage("TnmsMetric::deserialize()");

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

