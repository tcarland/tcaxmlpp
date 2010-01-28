/*
 * TnmsOid.h
 *
 *  Copyright(c) 2008,2009  Timothy Charlton Arland
 *  tcarland@gmail.com
 */
#define _TNMSCORE_TNMSOID_CPP_
#include <list>
#include <cstdlib>

#include "TnmsOid.h"

#include "Serializer.h"
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
uint32_t
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
    uint32_t * oid = NULL;
    uint32_t * rid = NULL;

    oid = (uint32_t*) ::calloc(_oidlist.size(), sizeof(uint32_t));
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


size_t
TnmsOid::getOidLength() const
{
    return _oidlist.size();
}


uint32_t
TnmsOid::lastValue() const
{
    if ( this->_oidlist.size() == 0 )
        return 0;
    return( _oidlist[_oidlist.size() - 1] );
}


ssize_t
TnmsOid::serialize ( char * buffer, size_t buffer_len ) const
{
    char   * wptr;
    size_t   wsz;
    ssize_t  pk, wt = 0;
    uint32_t oidsz;

    if ( buffer_len < this->size() )
        return 0;

    wptr  = buffer;
    wsz   = buffer_len;

    oidsz = (uint32_t) _oidlist.size();
    pk    = Serializer::Pack(wptr, (wsz - wt), oidsz);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    OidList::const_iterator  tIter;
    for ( tIter = _oidlist.begin(); tIter != _oidlist.end(); ++tIter ) {
        pk = Serializer::Pack(wptr, (wsz-wt), *tIter);
        if ( pk < 0 )
            return -1;
        wt    += pk;
        wptr  += pk;
    }

    return wt;
}


ssize_t
TnmsOid::deserialize ( const char * buffer, size_t buffer_len )
{
    const char * rptr;
    size_t       rsz, rd = 0;
    ssize_t      upk;
    uint32_t     oid_len, i;

    if ( buffer_len < this->size() )
        return -1;

    if ( ! _oidlist.empty() )
        _oidlist.clear();

    rptr  = buffer;
    rsz   = buffer_len;

    upk   = Serializer::Unpack(rptr, (rsz-rd), oid_len);
    if ( upk < 0 )
        return -1;

    rd   += upk;
    rptr += upk;

    rsz   = sizeof(uint32_t);

    for (  i = 0; i < oid_len; ++i ) {
        _oidlist.push_back( *((uint32_t*) rptr) );
        rptr += rsz;
        rd   += rsz;
    }

    return rd;
}


size_t
TnmsOid::size() const
{
    return(sizeof(uint32_t) + (_oidlist.size() * sizeof(uint32_t)));
}


void
TnmsOid::StringToOidList ( const std::string & str, OidList & oidlist )
{
    std::list<std::string>  branchNames;
    StringUtils::split(str, '.', std::back_inserter(branchNames));

    std::list<std::string>::iterator sIter;

    for ( sIter = branchNames.begin(); sIter != branchNames.end(); ++sIter )
        oidlist.push_back(StringUtils::fromString<uint32_t>(*sIter));

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
        oidlist.push_back(StringUtils::fromString<uint32_t>(*sIter));

    return oid;
}


/** OidFromOidIndex will take an existing TnmsOid and create a new parent oid
  * for or representing the given index. This is helpful for auto-generating 
  * parent tree objects when inserting a new Oid. Our tree has no restrictions
  * in allowing 'depth-first' objects within the tree.
 **/
TnmsOid*
TnmsOid::OidFromOidIndex ( const TnmsOid & oid, OidList::size_type & index )
{
    TnmsOid  * noid  = NULL;

    TnmsOid::const_iterator   tIter;
    OidList::size_type        bi  = 0;

    if ( oid.size() == 0 )
        return noid;

    noid = new TnmsOid();

    OidList & nlist = noid->getOidList();

    for ( tIter = oid.begin(); tIter != oid.end(), bi != (index+1); ++tIter, ++bi )
        nlist.push_back(*tIter);

    return noid;
}


} // namespace

// _TNMSCORE_TNMSOID_CPP_

