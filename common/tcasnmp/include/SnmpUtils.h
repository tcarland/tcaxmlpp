#ifndef _TCASNMP_SNMPUTILS_H_
#define _TCASNMP_SNMPUTILS_H_

#include <map>


namespace tcasnmp {

class SnmpSession;


class SnmpUtils {
    
public:
    
    static int          GetCDPNeighbors ( SnmpSession * session, 
                                          DeviceSet   & devlist );
    static void         MapInterfaces ( SnmpSession * session, 
                                        std::map<int, std::string> & ifmap );
    static void         QueryIpTable  ( SnmpSession * session,
                                        std::map<int, ipv4addr_t> & ipmap );

    /**  Queries the ifDescription of the given device's ifIndex. */
    static std::string  QueryIfDescr  ( SnmpSession * session, long indx );

    /**  Queries the ifAlias of the given device's ifIndex. */
    static std::string  QueryIfAlias  ( SnmpSession * session, long indx );



    
    
};

} // namespace


#endif /*_TCASNMP_SNMPUTILS_H_*/
