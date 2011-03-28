#define _HEXSTRINGFIELD_CPP_

#include "HexStringField.h"


namespace hexes {



HexStringField::HexStringField ( size_t max_len )
    : _maxLen(max_len),
      _curLen(0)
{
    this->initStringField();
}


HexStringField::~HexStringField()
{}


void
HexStringField::initStringField()
{
}

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

HexString::iterator
HexStringField::erase ( HexString::iterator index )
{
    _str.replace(*index, 1, 1, '_');
    index--;
    return index;
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
