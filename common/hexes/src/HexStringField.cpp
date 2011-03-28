#define _HEXSTRINGFIELD_CPP_

#include "HexStringField.h"


namespace hexes {


HexStringField::~HexStringField()
{}

HexString&
HexStringField::append ( const std::string & str )
{

    return *this;
}

HexString&
HexStringField::append ( const HexString & hexstr )
{

    return *this;
}

HexString&
HexStringField::append ( size_t count, char ch )
{

    return *this;
}

HexString&
HexStringField::assign ( const std::string & str )
{
    return *this;
}

HexString&
HexStringField::assign ( const HexString & str )
{
    return *this;
}

HexString&
HexStringField::assign ( size_t count, char ch )
{
    return *this;
}


} // namespace

// _HEXSTRINGFIELD_CPP_
