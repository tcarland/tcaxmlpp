#include <iostream>

#include "CidrUtils.h"
using namespace tcanetpp;

#include "FwVars.h"
#include "FwZones.h"
#include "FwRules.h"


using namespace fwgen;


int main ( int argc, char **argv )
{
    std::string varfile  = "etc/fwvars.cfg";
    std::string zonefile = "etc/ifws.cfg";
    std::string rulefile = "etc/fwrules.cfg";

    FwVars                 vars;
    FwVars::iterator       vIter;

    FwZones                zones;
    FwZones::iterator      zIter;

    FwDeviceMap::iterator  dIter;

    FwRules                rules;

    std::cout << std::endl 
        << " -------- fw variables --------" << std::endl;


    if ( ! vars.parse(varfile) ) {
        std::cout << "Parse error: " << vars.getErrorStr() << std::endl;
        return 0;
    }

    for ( vIter = vars.begin(); vIter != vars.end(); ++vIter )
        std::cout << vIter->first << " = "
            << CidrUtils::toString(vIter->second)
            << std::endl;



    std::cout << std::endl 
        << " -------- fw zones --------" << std::endl;


    //zones.setDebug(true);
    if ( ! zones.parse(zonefile) ) {
        std::cout << "Parse error: " << zones.getErrorStr() << std::endl;
        return 0;
    }

    for ( zIter = zones.begin(); zIter != zones.end(); ++zIter )
    {
        std::cout << std::endl << "Zone: " << zIter->first << "  (" 
            << zIter->second.deviceMap.size() << " devices)" << std::endl;
        if ( zIter->second.deviceMap.size() > 0 )
        {
            FwDeviceMap & dlist = zIter->second.deviceMap;

            for ( dIter = dlist.begin(); dIter != dlist.end(); ++dIter ) {
                std::cout << "  Device: " << dIter->first << "  ("
                    << dIter->second.portsmap.size() << " ports)" << std::endl;

                FwPortMap & pmap = dIter->second.portsmap;
                FwPortMap::iterator  pIter;

                for ( pIter = pmap.begin(); pIter != pmap.end(); ++pIter ) {
                    std::cout << pIter->second.portName << " : " 
                        << CidrUtils::toString(pIter->second.portAddr)
                        << " : " << pIter->second.isExternal << std::endl;
                }
            }
        }
    }


    std::cout << std::endl 
        << " -------- fw rules --------" << std::endl;


    rules.setDebug(true);
    if ( ! rules.parse(rulefile) ) {
        std::cout << "Parse error: " << rules.getErrorStr() << std::endl;
        return 0;
    }




    return 0;
}



