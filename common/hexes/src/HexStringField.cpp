/**
  * @file HexStringField.cpp
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
#define _HEXSTRINGFIELD_CPP_

#include "HexStringField.h"


namespace hexes {


//TODO: need display field length vs. max accepted.
//      scrolling within field


HexStringField::HexStringField ( size_t max_len )
    : _fld('_'),
      _maxLen(max_len),
      _curLen(0)
{
    _str.append(max_len, _fld);
}


HexStringField::HexStringField ( const HexString & str, size_t max_len )
    : HexString(str),
      _fld('_'),
      _maxLen(max_len)
{
    size_t   fill = 0;
    if ( str.length() > max_len )
        _str.erase(max_len);
    else
        fill = max_len - str.length();

    _curLen = _str.length();
    _str.append(fill, _fld);
}


HexStringField::HexStringField ( const std::string & str, size_t max_len )
    : HexString(str),
      _maxLen(max_len)
{
    size_t   fill = 0;
    if ( str.length() > max_len )
        _str.erase(max_len);
    else
        fill = max_len - str.length();

    _curLen = _str.length();
    _str.append(fill, _fld);
}


HexStringField::~HexStringField()
{}


void
HexStringField::operator= ( const HexString & hexstr )
{
    color      = hexstr.color;
    attributes = hexstr.attributes;
    alignment  = hexstr.alignment;
    wrap       = hexstr.wrap;
    this->assign(hexstr.str());
}

void
HexStringField::operator= ( const std::string & str )
{
    this->assign(str);
}


size_t
HexStringField::length() const
{
    return _curLen;
}

size_t
HexStringField::max_length() const
{
    return _maxLen;
}


HexString&
HexStringField::append ( const HexString & str )
{
    return this->append(str.str());
}


HexString&
HexStringField::append ( const std::string & str )
{
    std::string s = str;
    std::string::size_type indx = _curLen; // intentionally eos

    if ( s.length() + this->length() > _maxLen )
        s.erase(_maxLen - this->length());

    _str.replace(indx, _str.length(), s);
    _curLen += s.length();

    return *this;
}


HexString&
HexStringField::append ( size_t count, char ch )
{
    if ( ! HexString::CharIsVisible(ch) )
        return *this;

    std::string::size_type  indx, cnt;

    indx = _curLen;     // intentionally eos
    cnt  = _maxLen - _curLen;

    if ( count < cnt ) // write what we can
        cnt = count;

    _str.replace(indx, 1, cnt, ch);
    _curLen += cnt;

    return *this;
}


HexString&
HexStringField::assign ( const HexString & str )
{
    return this->assign(str.str());
}


HexString&
HexStringField::assign ( const std::string & str )
{
    std::string s = str;
    size_t   fill = 0;

    if ( s.length() > _maxLen )
        s.erase(_maxLen);
    else if ( s.length() < _maxLen )
        fill = _maxLen - s.length();

    _curLen = s.length();
    s.append(fill, _fld);
    _str.assign(s);

    return *this;
}


HexString&
HexStringField::assign ( size_t count, char ch )
{
    size_t fill = 0;
    size_t cnt  = count;

    if ( cnt > _maxLen )
        cnt = _maxLen;

    fill  = _maxLen - cnt;
    _curLen = cnt;
    _str.assign(count, ch);
    _str.append(fill, _fld);

    return *this;
}


HexString&
HexStringField::erase ( size_type from, size_type to )
{
    _str.replace(from, (to-from), 1, _fld);
    return *this;
}


HexString::iterator
HexStringField::erase ( HexString::iterator start, HexString::iterator end )
{
    _str.replace(start, end, 1, _fld);
    start--;
    return start;
}


} // namespace

// _HEXSTRINGFIELD_CPP_
