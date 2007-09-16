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
    std::string        ipstr, id, comm;

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

    printf("Results for device %s\n", ipstr.c_str());

    printf("ifmap size is %d\n", ifmap.size());

    for ( fIter = ifmap.begin(); fIter != ifmap.end(); fIter++ )
	std::cout << fIter->first << " : " << fIter->second << std::endl;

    std::cout << std::endl << "Finished" << std::endl;

    snmp->closeSession();
    //delete snmp;

    /*-------------*/

    NetworkDevice  dev(ipstr, comm);

    snmp->openSession(dev);
    //SnmpCDP* cdp = new SnmpCDP(dev);
    //cdp->setDebug(true);

    DeviceSet  v;
    DeviceSet::iterator vIter;


    printf("devset size is %d\n", v.size());

    SnmpUtils::GetCDPNeighbors(snmp, v);

    for ( vIter = v.begin(); vIter != v.end(); vIter++ ) {
	NetworkDevice device = (NetworkDevice) *vIter;
	std::string name = CidrUtils::toString(device.deviceAddr());
	printf("DeviceName: %s  addr: %s\n", device.deviceName().c_str(), name.c_str());
    }

    snmp->closeSession();
    delete snmp;
    //delete cdp;
    

    return 0;
}



void usage()
{
    printf("Usage: snmptest rtripstr community\n\n");
    printf("  Eg: snmptest 64.37.155.240 mysnmpcommunity\n\n");
    exit(0);
}


