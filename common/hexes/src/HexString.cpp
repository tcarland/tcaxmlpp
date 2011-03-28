#define _HEXES_HEXSTRING_CPP_

#include "HexString.h"


namespace hexes {


HexString::HexString()     
    : color(0),
      attributes(0),
      alignment(0),
      wrap(true)
{}

HexString::HexString ( const std::string & str_, 
                       bool  wrap_ )
    : color(0),
      attributes(0),
      alignment(0),
      wrap(wrap_),
      _str(str_)
{}


HexString::HexString ( const std::string & str_, 
                       int   color_,
                       int   attributes_,
                       bool  wrap_ )
    : color(color_),
      attributes(attributes_),
      alignment(0),
      wrap(wrap_),
      _str(str_)
{}  

HexString::HexString ( const HexString & hexstr )
{
    _str       = hexstr._str;
    color      = hexstr.color;
    attributes = hexstr.attributes;
    alignment  = hexstr.alignment;
    wrap       = hexstr.wrap;
}

HexString::~HexString() {}


void 
HexString::operator= ( const HexString & hexstr )
{
    _str       = hexstr._str;
    color      = hexstr.color;
    attributes = hexstr.attributes;
    alignment  = hexstr.alignment;
    wrap       = hexstr.wrap;
}

void
HexString::operator= ( const std::string & str )
{
    _str       = str;
}

bool
HexString::operator== ( const HexString & hexstr )
{
    if ( _str.compare(hexstr._str) == 0 )
        return true;
    return false;
}

const std::string&
HexString::str() const
{
    return _str;
}

std::string&
HexString::str()
{
    return _str;
}

size_t
HexString::length() const
{
    return((size_t)_str.length());
}

bool
HexString::empty() const
{
    return(_str.empty());
}

HexString&
HexString::append ( const std::string & str )
{
    this->_str.append(str);
    return *this;
}

HexString&
HexString::append ( const HexString & str )
{
    this->_str.append(str.str());
    return *this;
}

HexString&
HexString::append ( size_t count, char ch )
{
    this->_str.append(count, ch);
    return *this;
}

HexString&
HexString::assign ( const std::string & str )
{
    this->_str.assign(str);
    return *this;
}

HexString&
HexString::assign ( const HexString & str )
{
    this->_str.assign(str.str());
    return *this;
}

HexString&
HexString::assign ( size_t count, char ch )
{
    this->_str.assign(count, ch);
    return *this;
}

HexString::iterator
HexString::erase ( HexString::iterator index )
{
    return this->_str.erase(index);
}

HexString::iterator
HexString::erase ( HexString::iterator start, HexString::iterator end )
{
    return this->_str.erase(start, end);
}


bool
HexString::CharIsVisible ( char ch )
{
    if ( ch > 31 && ch < 127 )
        return true;
    return false;
}


}  // namespace

// _HEXES_HEXSTRING_CPP_
