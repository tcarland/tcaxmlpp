#ifndef _TCASNMP_SNMPOID_H_
#define _TCASNMP_SNMPOID_H_

#include <string>

namespace tcasnmp {


class SnmpOid {
    
public:

    SnmpOid();
    SnmpOid  ( const oid * anOid, size_t oid_len );
    SnmpOid  ( const std::string & oidstr );
    SnmpOid  ( const SnmpOid & noid );

    virtual ~SnmpOid();


    void         operator=   ( const SnmpOid & noid );
    bool         operator==  ( const SnmpOid & noid ) const;
    bool         operator<   ( const SnmpOid & noid ) const;


    bool         hasOid      ( const oid * anOid, size_t oid_len ) const;
    oid*         getOid();
    size_t       getOidLen();
    std::string  getOidStr();


    static
    std::string  StringFromBitString ( const std::string & bitstr );
    static
    std::string  StringToBitString   ( const std::string & str );


  protected:

    static
    std::string  OidToString ( const oid * anOid, size_t oid_len );

    static
    oid*         StringToOid ( const std::string & oidstr, 
                               size_t & oid_len );

  private:

    oid *              _theOid;
    size_t             _oidLen;

    std::string        _oidStr;

};

} // namespace

#endif /*_TCASNMP_SNMPOID_H_*/

