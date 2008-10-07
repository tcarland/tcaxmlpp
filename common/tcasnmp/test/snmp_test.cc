#define SNMP_TEST_CC

#include <map>
#include <string>
#include <iostream>


#include "SnmpSession.h"
#include "SnmpUtils.h"
using namespace tcasnmp;

#include "CidrUtils.h"
using namespace tcanetpp;


void usage();


int main ( int argc, char **argv )
{
    SnmpSession * snmp = NULL;
    std::string   ipstr, id, comm;

    if ( argc != 3 )
	usage();

    ipstr = argv[1];
    comm  = argv[2];

    snmp = new SnmpSession();
    //snmp->setDebug(true);

    snmp->openSession(ipstr, comm);

    InterfaceMap            ifmap;
    InterfaceMap::iterator  fIter;

    SnmpUtils::MapInterfaces(snmp, ifmap);

    std::cout << "Results for device " << ipstr << std::endl;
    std::cout << "ifmap size :" << ifmap.size() << std::endl;

    for ( fIter = ifmap.begin(); fIter != ifmap.end(); fIter++ )
	std::cout << fIter->first << " : " << fIter->second << std::endl;

    std::cout << std::endl << "Finished" << std::endl;

    snmp->closeSession();

    /*-------------*/

    NetworkDevice  dev(ipstr, comm);

    snmp->openSession(dev);

    DeviceSet            v;
    DeviceSet::iterator  vIter;

    std::cout << "device set == " << v.size() << std::endl;

    SnmpUtils::GetCDPNeighbors(snmp, v);

    for ( vIter = v.begin(); vIter != v.end(); vIter++ )
        std::cout << "DeviceName: " << vIter->deviceName() 
                  << "  addr: " << CidrUtils::toString(vIter->deviceAddr()) << std::endl;

    snmp->closeSession();

    delete snmp;
    
    return 0;
}



void usage()
{
    printf("Usage: snmptest rtripstr community\n\n");
    printf("  Eg: snmptest 64.37.155.240 mysnmpcommunity\n\n");
    exit(0);
}


