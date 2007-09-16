#define _TCASNMP_SNMPOID_CC_

#include <sstream>

#include "SnmpOid.h"

#include "StringUtils.h"
using namespace tcanetpp;


namespace tcasnmp {


SnmpOid::SnmpOid()
    : _theOid(NULL),
      _oidLen(0)
{
    SnmpSession::LibInit();
}


SnmpOid::SnmpOid ( const oid * anOid, size_t oid_len )
    : _theOid(NULL),
      _oidLen(0)
{
    SnmpSession::LibInit();

    _theOid = snmp_duplicate_objid(anOid, oid_len);
    _oidLen = oid_len;
    _oidStr = SnmpOid::OidToString(_theOid, _oidLen);
}


SnmpOid::SnmpOid ( const std::string & oidstr )
    : _theOid(NULL),
      _oidLen(0)
{
    SnmpSession::LibInit();

    _theOid = SnmpOid::StringToOid(oidstr, _oidLen);
    _oidStr = SnmpOid::OidToString(_theOid, _oidLen);
}


SnmpOid::SnmpOid ( const SnmpOid & noid )
    : _theOid(NULL),
      _oidLen(noid._oidLen),
      _oidStr(noid._oidStr)
{
    _theOid = snmp_duplicate_objid(noid._theOid, noid._oidLen);
}


SnmpOid::~SnmpOid()
{
    if ( _theOid )
        SNMP_FREE(_theOid);
} 



void
SnmpOid::operator= ( const SnmpOid & noid ) 
{
    _oidStr = noid._oidStr;
    _oidLen = noid._oidLen;
    _theOid = snmp_duplicate_objid(noid._theOid, noid._oidLen);
}

bool
SnmpOid::operator== ( const SnmpOid & noid ) const
{
    return(snmp_oid_compare(_theOid, _oidLen, noid._theOid, noid._oidLen) == 0);
}


bool
SnmpOid::operator< ( const SnmpOid & noid ) const
{
    return(snmp_oid_compare(_theOid, _oidLen, noid._theOid, noid._oidLen ) < 0);
}


bool
SnmpOid::hasOid ( const oid * anOid, size_t oid_len ) const
{
    return(netsnmp_oid_is_subtree(_theOid, _oidLen, anOid, oid_len) == 0);
}


oid*
SnmpOid::getOid()
{
    return _theOid;
}

size_t
SnmpOid::getOidLen()
{
    return _oidLen;
}

std::string
SnmpOid::getOidStr()
{
    return _oidStr;
}


std::string
SnmpOid::OidToString ( const oid * anOid, size_t oid_len )
{
    std::string  oidstr;
    char         coid[MAX_OID_LEN];

    if ( anOid == NULL )
        return oidstr;

    snprint_objid(coid, MAX_OID_LEN, anOid, oid_len);
    oidstr.assign(coid);

    return oidstr;
}


oid*
SnmpOid::StringToOid ( const std::string & oidstr, size_t & oid_len )
{
    size_t    oidLen = MAX_OID_LEN;
    oid     * theoid = NULL;
    oid       anOid[oidLen];

    snmp_parse_oid(oidstr.c_str(), anOid, &oidLen);

    oid_len = oidLen;
    theoid  = snmp_duplicate_objid(anOid, oid_len);

    return theoid;
}

    
std::string
SnmpOid::StringFromBitString ( const std::string & bitstr )
{
    std::string   str;
    std::vector<std::string>  v;
    std::vector<std::string>::iterator vIter;

    StringUtils:split(bitstr, '.', std::back_inserter(v));

    if ( v.size() == 0 )
        return str;

    v.erase(v.begin());

    for ( vIter = v.begin(); vIter != v.end(); ++vIter ) {
        char a  = (char) StringUtils::fromString<int>(*vIter);
        str.append(1, a);
    }

    return str;
}


std::string
SnmpOid::StringToBitString ( const std::string & str )
{
    std::ostringstream  bitstr;

    if ( str.empty() )
        return bitstr.str();

    bitstr << str.length();

    for ( u_int i = 0; i < str.length(); ++i )
        bitstr << "." << (int) str.at(i);

    return bitstr.str();
}


} // namespace


/*  _TCASNMP_SNMPOID_CC_  */
