#ifndef _TCASNMP_SNMPPDU_H_
#define _TCASNMP_SNMPPDU_H_

#include <string>

#include "SnmpOid.h"
#include "tcanetpp_types.h"

namespace tcasnmp {


class SnmpPdu {
    
public:
    
    SnmpPdu();
    SnmpPdu ( struct snmp_pdu * pdu_ );

    virtual ~SnmpPdu();

    /**  Converts the snmp_pdu objectId to a string on success and 
      *  returns false if the pdu is invalid.
     **/
    bool     getOid               ( std::string & soid );

    /**   Returns the SNMP PDU response type.     **/
    bool     getResponseType      ( char & type );
    
    /**  Converts the PDU response packet to the corresponding type
      *  and returns the success of that operation */
    bool     getString            ( std::string & val );
    bool     getUnsignedInteger   ( uint32_t    & val );
    bool     getInteger           ( int         & val );
    bool     getTimestamp         ( time_t      & ts );
    bool     getObjectId          ( std::string & soid );


  public:

    struct snmp_pdu *   pdu;

};


} // namespace

#endif // _TCASNMP_SNMPPDU_H_


