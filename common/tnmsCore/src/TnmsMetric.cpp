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
    : _oid(NULL)
{}

TnmsMetric::TnmsMetric ( const std::string & name, TnmsOid * oid )
    : _name(name),
      _oid(oid)
{}


const std::string&
TnmsMetric::getName() const
{
    return _name;
}


const TnmsOid&
TnmsMetric::getOid() const
{
    return *_oid;
}


std::string
TnmsMetric::getValue()
{
    return _valueStr;
}


template<typename T>
T&
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
TnmsMetric::serialize ( char * buffer, size_t  size )
{
    ssize_t   pk, wt = 0;

    char * wptr = buffer;

    pk    = Packer::pack(wptr, size - wt, (uint32_t) _oid->size());
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = _oid->serialize(wptr, (size - wt));
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = Packer::pack(wptr, (size - wt), _name);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk  = Packer::pack(wptr, (size - wt), (uint16_t) _valueType);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    if ( _valueType == TNMS_STRING )
        pk = Packer::pack(wptr, (size -wt), _valueStr);
    else
        pk = Packer::pack(wptr, (size-wt), _value);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    // TO-DO: pvt data
    std::string  pvt;

    pk   = Packer::pack(wptr, (size - wt), pvt);
    if ( pk < 0 )
        return -1;

    return wt;
}


} // namespace


// _TNMSMETRIC_CPP_
