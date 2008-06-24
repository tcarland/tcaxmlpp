#define _TNMSOID_CPP_


#include "TnmsOid.h"


namespace tnmsCore {


TnmsOid::TnmsOid()
{}

TnmsOid::TnmsOid ( const OidList & oidlist )
{
    this->_oid  = TnmsOid::CreateNewOid(oidlist.size());
    tnmsOid ptr = this->_oid.oid;

    OidList::iterator  tIter;
    for ( int i = 0, tIter = oidlist.begin(); tIter != oidlist.end(); ++tIter ) {
        uint16_t val = *tIter;

        ptr  = ptr + i;
        *ptr = val;
        i   += sizeof(val);
    }

}

TnmsOid::TnmsOid ( const std::string & oid  )
{
}


TnmsOid::TnmsOid ( tnmsOid oid )
{
}


TnmsOid::~TnmsOid()
{
    if ( _oid.oid )
        ::free(_oid.oid);
}

uint16_t
TnmsOid::operator[] ( uint32_t indx )
{
    if ( indx >= _oid.oidlen )
        return 0;
    
    return( *(_oid.oid + indx) );
}


std::string
TnmsOid::toString() const
{
    std::string  oidstr;
    uint32_t     indx;
    
    oidsstr.append(StringUtils::toString(*_oid.oid));
    
    for ( indx = 1; indx != _oid.oidlen; ++indx ) {
        oidsstr.append(".");
        oidstr.append(StringUtils::toString(*(_oid.oid + indx)));
    }
    
    return oidstr;
}


void
TnmsOid::toOidArray ( OidList & oidlist ) const 
{
    uint32_t indx;
    
    for ( indx = 0; indx != _oid.oidlen; ++indx )
        oidlist.push_back( *(this->_oid.oid + indx) );
    
    return;
}


const OidList&
TnmsOid::getOidList() const 
{
    return _oid;
}


OidList&
TnmsOid::getOidList()
{
    return _oid;
}


uint32_t
TnmsOid::getOidLength() const
{
    return _oid.size();
}


size_t    
TnmsOid::serialize ( char * buffer, size_t len )
{
    if ( len < (sizeof(uint32_t) + (_oid.oidlen * sizeof(uint16_t))) )
        return 0;

    size_t  sz  = sizeof(uint16_t);
    char * bptr = buffer;
    
    Oidlist::iterator  tIter;

    for ( tIter = _oid.begin(); tIter != _oid.end(); ++tIter ) {
        *bptr   = *tIter;
        bptr   += sz;
    }

    sz = sz * _oid.size();

    return sz;
}


} // namespace

// _TNMSOID_CPP_

