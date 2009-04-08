/**
  *
  *
  * Copyright (c) 2008, 2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
  *
  * This file is part of Foobar.
  *
  * Foobar is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * Foobar is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with Foobar.  
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#ifndef _TCASNMP_SNMPOID_H_
#define _TCASNMP_SNMPOID_H_

#include <string>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>


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

