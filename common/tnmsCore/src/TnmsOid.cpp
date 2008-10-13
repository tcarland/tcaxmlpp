#define _TNMSOID_CPP_
#include <list>

#include "TnmsOid.h"

#include "StringUtils.h"
using namespace tcanetpp;


namespace tnmsCore {



TnmsOid::TnmsOid ( OidList::size_type  size )
    : _oidlist(OidList(size))
{}


TnmsOid::TnmsOid ( const OidList & oidlist )
    : _oidlist(oidlist)
{}


TnmsOid::TnmsOid ( const std::string & oidstr  )
{
    TnmsOid::StringToOidList(oidstr, this->_oidlist);
}


TnmsOid::TnmsOid ( const TnmsOid & toid )
    : _oidlist(toid.getOidList())
{}


TnmsOid::~TnmsOid()
{
    this->_oidlist.clear();
}


/** should probably throw */
uint16_t
TnmsOid::operator[] ( OidList::size_type  indx ) const
{
    if ( indx >= _oidlist.size() )
        return 0;
    return( _oidlist[indx] );
}


bool
TnmsOid::operator< ( const TnmsOid & toid ) const
{
    return(this->lastValue() < toid.lastValue());
}

void
TnmsOid::operator= ( const TnmsOid & toid )
{
    this->_oidlist = toid.getOidList();
}


TnmsOid::iterator
TnmsOid::begin()
{
    return _oidlist.begin();
}

TnmsOid::iterator
TnmsOid::end()
{
    return _oidlist.end();
}


TnmsOid::const_iterator
TnmsOid::begin() const
{
    return _oidlist.begin();
}

TnmsOid::const_iterator
TnmsOid::end() const
{
    return _oidlist.end();
}


bool
TnmsOid::empty() const
{
    return _oidlist.empty();
}


std::string
TnmsOid::toString() const
{
    std::string              oidstr;
    OidList::const_iterator  tIter;

    tIter  = _oidlist.begin();

    if ( tIter == _oidlist.end() )
    	return oidstr;

    oidstr = StringUtils::toString(*tIter++);

    for ( ; tIter != _oidlist.end(); ++tIter ) {
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

    oid = (uint16_t*) ::calloc(_oidlist.size(), sizeof(uint16_t));
    rid = oid;

    OidList::const_iterator  tIter;
    for ( tIter = _oidlist.begin(); tIter != _oidlist.end(); ++tIter, ++rid )
        *rid = *tIter;

    return oid;
}


const OidList&
TnmsOid::getOidList() const
{
    return _oidlist;
}


OidList&
TnmsOid::getOidList()
{
    return _oidlist;
}


uint32_t
TnmsOid::getOidLength() const
{
    return _oidlist.size();
}


uint16_t
TnmsOid::lastValue() const
{
    if ( this->_oidlist.size() == 0 )
        return 0;
    return( _oidlist[_oidlist.size() - 1] );
}


size_t
TnmsOid::serialize ( char * buffer, size_t len )
{
    if ( len < (sizeof(uint32_t) + (_oidlist.size() * sizeof(uint16_t))) )
        return 0;

    size_t  sz  = sizeof(uint16_t);
    char * bptr = buffer;

    OidList::iterator  tIter;

    for ( tIter = _oidlist.begin(); tIter != _oidlist.end(); ++tIter ) {
        *bptr   = *tIter;
        bptr   += sz;
    }

    sz = sz * _oidlist.size();

    return sz;
}


void
TnmsOid::StringToOidList ( const std::string & str, OidList & oidlist )
{
    std::list<std::string>  branchNames;
    StringUtils::split(str, '.', std::back_inserter(branchNames));

    std::list<std::string>::iterator sIter;

    for ( sIter = branchNames.begin(); sIter != branchNames.end(); ++sIter )
        oidlist.push_back(StringUtils::fromString<uint16_t>(*sIter));

    return;
}


TnmsOid*
TnmsOid::OidFromString ( const std::string & str )
{
    TnmsOid   * oid = NULL;

    std::list<std::string>  branchNames;
    OidList::size_type      sz;

    StringUtils::split(str, '.', std::back_inserter(branchNames));

    if ( branchNames.size() == 0 )
        return NULL;

    sz  = (OidList::size_type) branchNames.size();
    oid = new TnmsOid(sz);

    OidList & oidlist = oid->getOidList();

    std::list<std::string>::iterator   sIter;
    for ( sIter = branchNames.begin(); sIter != branchNames.end(); ++sIter )
        oidlist.push_back(StringUtils::fromString<uint16_t>(*sIter));

    return oid;
}


TnmsOid*
TnmsOid::OidFromIndex ( const TnmsOid & oid, OidList::size_type & indx )
{
    TnmsOid  * noid  = NULL;

    TnmsOid::const_iterator   tIter;
    OidList::size_type        bi  = 0;

    if ( oid.size() == 0 )
        return noid;

    noid = new TnmsOid();

    OidList & nlist = noid->getOidList();

    for ( tIter = oid.begin(); tIter != oid.end(), bi != (indx+1); ++tIter, ++bi )
        nlist.push_back(*tIter);

    return noid;
}


} // namespace

// _TNMSOID_CPP_

