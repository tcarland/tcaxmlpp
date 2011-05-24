/**
  * @file HexString.cpp
  *
  * Copyright (c) 2011 Timothy Charlton Arland
  * @author tca@charltontechnology.net
  *
  * @section LICENSE
  *
  * This file is part of libhexes.
  *
  * libhexes is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as
  * published by the Free Software Foundation, either version 3 of
  * the License, or (at your option) any later version.
  *
  * libhexes is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with libhexes.
  * If not, see <http://www.gnu.org/licenses/>.
 **/
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

//----------------------------------------------------------------//

void
HexString::operator= ( const std::string & str )
{
    _str  = str;
}

bool
HexString::operator== ( const HexString & hexstr )
{
    if ( _str.compare(hexstr._str) == 0 )
        return true;
    return false;
}

//----------------------------------------------------------------//

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

//----------------------------------------------------------------//

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
HexString::append ( size_type count, char ch )
{
    this->_str.append(count, ch);
    return *this;
}

//----------------------------------------------------------------//

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
HexString::assign ( size_type count, char ch )
{
    this->_str.assign(count, ch);
    return *this;
}

//----------------------------------------------------------------//

HexString&
HexString::erase ( HexString::size_type from, HexString::size_type to )
{
    this->_str.erase(from, to);
    return *this;
}

HexString::iterator
HexString::erase ( HexString::iterator start, HexString::iterator end )
{
    return this->_str.erase(start, end);
}

//----------------------------------------------------------------//

bool
HexString::CharIsVisible ( char ch )
{
    if ( ch > 31 && ch < 127 )
        return true;
    return false;
}

int
HexString::IndexOf ( const std::string & str, const std::string & match, size_t from )
{
    std::string::size_type  indx;

    if ( from > str.length() || from < 0 )
        return -1;

    if ( (indx = str.find(match, from)) == std::string::npos )
        return -1;

    return((int) indx);
}

//----------------------------------------------------------------//

}  // namespace

// _HEXES_HEXSTRING_CPP_
