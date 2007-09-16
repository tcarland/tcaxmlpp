/**  SnmpSession - This derives from SnmpSessionInterface and implements 
  *   the SNMP layer via the net-snmp library.
 **/
#ifndef _TCASNMP_SNMPSESSION_H_
#define _TCASNMP_SNMPSESSION_H_

#include <string>


#include "NetworkDevice.h"
using namespace tcanetpp;

#include "SnmpOid.h"
#include "SnmpPdu.h"


namespace tcasnmp {


class SnmpSession {
    
public:
    
    SnmpSession();
    SnmpSession ( const std::string & host, const std::string & community );
    SnmpSession ( const NetworkDevice & device );
    
    virtual ~SnmpSession();
    
    
    virtual bool                 openSession   ( const std::string & host,
                                                 const std::string & community );
    virtual bool                 openSession   ( const NetworkDevice & device,
                                                 bool write = false );
    
    virtual void                 closeSession();
    
    virtual SnmpPdu*             get       ( const std::string & oidstr );
    virtual SnmpPdu*             get       ( SnmpOid   & oid );
    
    virtual SnmpPdu*             getNext   ( const std::string & oidstr );
    virtual SnmpPdu*             getNext   ( SnmpOid  & oid );
    
    virtual SnmpPdu*             getNext   ( SnmpPdu  * response );
    
    
    /**  Performs the snmpset operation for the provided OID, type
      *  and value. The types are standard snmp OID value types
      *  as defined and use by net-snmp:
      *      i  Integer,
      *      u  Unsigned,
      *      s  String, 
      *      x  Hex String,
      *      d  Decimal String,
      *      n  Null object,
      *      o  Objid,
      *      t  Timeticks,
      *      a  IP Address, and
      *      b  Bits
     **/
    virtual bool                 set       ( const std::string & soid, char type,
                                             const std::string & value );
    
    virtual const std::string &  getErrorStr();
    virtual void                         setErrorStr ( const std::string & errstr ) { _errStr = errstr; }
    
    /*  Some common mib-2 snmp device queries  */
    virtual std::string          getSysDescr();
    virtual std::string          getSysName();
    virtual time_t               getSysUpTime();

    std::string                  community();


    /**  Static function to ensure net-snmp library has been properly 
      *  initialized
     **/
    static  void                 LibInit(); 
    
    /**  Releases(free's) a net-snmp PDU object */
    static void                  ReleasePDU    ( SnmpPdu * pdu );


protected:

  /**  Ensures net-snmp library is initialized if necessary, and
    *  creates the session instance object of the net-snmp 
    *  single-session api.
   **/
  void         init();
  
  /**  Returns a boolean indicating whether the provided pdu is 
    *  still part of the base OID that was initially queried */
  bool         hasBaseOid    ( struct snmp_pdu * response );


  struct snmp_session          _session;
  void*                        _sptr;
  oid*                         _baseOid;
  size_t                       _baseLen;
  std::string                  _community;
  std::string                  _errStr;


private:

  static bool                  _NetSnmpLibInit;

};


}  // namespace

#endif /*_TCASNMP_NETSNMPSESSION_H_*/
