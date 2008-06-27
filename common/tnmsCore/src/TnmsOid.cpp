#define _TNMSOID_CPP_
#include <list>
#include <iostream>

#include "TnmsOid.h"

#include "StringUtils.h"
using namespace tcanetpp;

namespace tnmsCore {


TnmsOid::TnmsOid()
    : _oidlist(new OidList())
{}

TnmsOid::TnmsOid ( const OidList & oidlist )
    : _oidlist(new OidList(oidlist))
{
}

TnmsOid::TnmsOid ( const std::string & oid  )
    : _oidlist(new OidList())
{
    TnmsOid::StringToOidList(oid, *this->_oidlist);
}



TnmsOid::~TnmsOid()
{
    if ( this->_oidlist )
        delete this->_oidlist;
}

/* this is painful and should be removed */
uint16_t
TnmsOid::operator[] ( uint32_t indx )
{
    if ( indx >= (_oidlist->size() - 1) )
        return 0;

    OidList::iterator tIter = _oidlist->begin();
    
    uint32_t i = 0;
    while ( i < indx ) {
        ++tIter;
        ++i;
    }
    
    return( *tIter );
}

TnmsOid::iterator
TnmsOid::begin()
{
    return _oidlist->begin();
}

TnmsOid::iterator
TnmsOid::end()
{
    return _oidlist->end();
}

TnmsOid::const_iterator
TnmsOid::begin() const 
{
    return _oidlist->begin();
}

TnmsOid::const_iterator
TnmsOid::end() const
{
    return _oidlist->end();
}

bool
TnmsOid::empty() const
{
    return _oidlist->empty();
}


std::string
TnmsOid::toString() const
{
    std::string  oidstr;
   
    OidList::iterator  tIter = _oidlist->begin();
    oidstr.append(StringUtils::toString(*tIter++));
    
    for ( ; tIter != _oidlist->end(); ++tIter ) {
        oidstr.append(".");
        oidstr.append(StringUtils::toString(*tIter));
    }
    
    return oidstr;
}


tOid
TnmsOid::toArray() const 
{
    uint16_t * oid = NULL;
    uint16_t * rid = NULL;

    oid = (uint16_t*) ::calloc(_oidlist->size(), sizeof(uint16_t));
    rid = oid;

    OidList::iterator tIter;
    for ( tIter = _oidlist->begin(); tIter != _oidlist->end(); ++tIter, ++rid )
        *rid = *tIter;
    
    return oid;
}

const OidList&
TnmsOid::getOidList() const
{
    return *_oidlist;
}


OidList&
TnmsOid::getOidList()
{
    return *_oidlist;
}


uint32_t
TnmsOid::getOidLength() const
{
    return _oidlist->size();
}


const uint16_t&
TnmsOid::lastValue() const
{
    return((*_oidlist)[_oidlist->size() - 1]);
}


size_t    
TnmsOid::serialize ( char * buffer, size_t len )
{
    if ( len < (sizeof(uint32_t) + (_oidlist->size() * sizeof(uint16_t))) )
        return 0;

    size_t  sz  = sizeof(uint16_t);
    char * bptr = buffer;
    
    OidList::iterator  tIter;

    for ( tIter = _oidlist->begin(); tIter != _oidlist->end(); ++tIter ) {
        *bptr   = *tIter;
        bptr   += sz;
    }

    sz = sz * _oidlist->size();

    return sz;
}

void
TnmsOid::StringToOidList ( const std::string & str, OidList & oidlist )
{
    std::cout << "TnmsOid::StringToOid() '" << str << "'" << std::endl;

    std::list<std::string>  branchNames;
    StringUtils::split(str, '.', std::back_inserter(branchNames));

    std::list<std::string>::iterator sIter;

    for ( sIter = branchNames.begin(); sIter != branchNames.end(); ++sIter )
        oidlist.push_back(StringUtils::fromString<uint16_t>(*sIter));

    /*
    std::string::size_type  begin = 0, end = 0;
    char delimiter = '.';

    while ( (begin = str.find_first_not_of(delimiter, begin)) != std::string::npos )
    {
        end     = str.find_first_of(delimiter, begin);
        oidlist.push_back(StringUtils::fromString<uint16_t>(str.substr(begin, end - begin)));
        begin   = end;
    }
    */

    return;
}


} // namespace

// _TNMSOID_CPP_

