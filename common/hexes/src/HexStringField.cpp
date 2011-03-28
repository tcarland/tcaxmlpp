#define _HEXSTRINGFIELD_CPP_

#include "HexStringField.h"


namespace hexes {



HexStringField::HexStringField ( size_t max_len )
    : _maxLen(max_len),
      _curLen(0)
{
    _str.append(max_len, '_');
}

HexStringField::HexStringField ( const HexString & str, size_t max_len )
    : HexString(str)
{
    size_t   fill = 0;
    if ( str.length() > max_len )
        _str.erase(max_len);
    else
        fill = max_len - str.length();

    _str.append(fill, '_');
}

HexStringField::HexStringField ( const std::string & str, size_t max_len )
    : HexString(str)
{
    size_t   fill = 0;
    if ( str.length() > max_len )
        _str.erase(max_len);
    else
        fill = max_len - str.length();

    _str.append(fill, '_');
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
    return _str.length();
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

    return *this;
}

/* this is fun */
HexString&
HexStringField::append ( size_t count, char ch )
{
    if ( ! HexString::CharIsVisible(ch) )
        return *this;

    std::string::size_type  indx, cnt;

    indx = _curLen; // intentionally eos
    cnt  = _maxLen - _curLen;

    if ( count < cnt ) // write what we can
        cnt = count;

    _str.replace(indx, 1, cnt, ch);

    return *this;
}


HexString&
HexStringField::assign ( const HexString & str )
{
    return this->assign(str.str());
}

/* wheee */
HexString&
HexStringField::assign ( const std::string & str )
{
    std::string s = str;

    if ( s.length() > _maxLen )
        s.erase(_maxLen);
    else if ( s.length() < _maxLen )
        s.append((_maxLen - s.length()), '_');

    _str.assign(s);

    return *this;
}

HexString&
HexStringField::assign ( size_t count, char ch )
{
    _str.assign(count, ch);
    return *this;
}

HexString&
HexStringField::erase ( size_type from, size_type to )
{
    _str.replace(from, (to-from), 1, '_');
    return *this;
}

HexString::iterator
HexStringField::erase ( HexString::iterator start, HexString::iterator end )
{
    _str.replace(start, end, 1, '_');
    start--;
    return start;
}


} // namespace

// _HEXSTRINGFIELD_CPP_
