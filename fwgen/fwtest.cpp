#include <iostream>

#include "CidrUtils.h"
using namespace tcanetpp;

#include "FwVars.h"
#include "FwZones.h"


using namespace fwgen;


int main ( int argc, char **argv )
{
    std::string varfile  = "etc/fwvars.cfg";
    std::string zonefile = "etc/ifws.cfg";

    FwVars            vars;
    FwZones  zones;
    FwVars::iterator  vIter;

    if ( ! vars.parse(varfile) ) {
        std::cout << "Parse error: " << vars.getErrorStr() << std::endl;
        return 0;
    }

    for ( vIter = vars.begin(); vIter != vars.end(); ++vIter )
        std::cout << vIter->first << " = " 
            << CidrUtils::toCidrString(vIter->second)
            << std::endl;


    if ( ! zones.parse(zonefile) ) {
        std::cout << "Parse error: " << zones.getErrorStr() << std::endl;
        return 0;
    }

/*
    FwVarMap            varmap;
    FwVarMap::iterator  vIter;

    FwConfig::ParseFwVars(varfile, varmap);

    for ( vIter = varmap.begin(); vIter != varmap.end(); ++vIter ) {
        std::cout << "fwvar: " << vIter->first << " == " 
            << CidrUtils::toCidrString(vIter->second) << std::endl;
    }

    FwZoneMap            zonemap;
    FwZoneMap::iterator  zIter;
    FwDevList::iterator  dIter;

    FwConfig::ParseFwInterfaces("etc/ifws.cfg", zonemap);

    for ( zIter = zonemap.begin(); zIter != zonemap.end(); ++zIter ) {
        FwZone    * zone  = zIter->second;
        FwDevList & dlist = zone->deviceList;

        std::cout << "Zone: " << zone->zoneName << std::endl;

        for ( dIter = dlist.begin(); dIter != dlist.end(); ++dIter ) {
            FwDevice              * dev   = *dIter;
            FwIfaceList           & flist = dev->ifaceList;
            FwIfaceList::iterator   fIter;

            std::cout << " device: " << dev->deviceName << std::endl;

            for ( fIter = flist.begin(); fIter != flist.end(); ++fIter ) {
                FwIface * iface = *fIter;
                std::cout << iface->ifName << " : "
                    << CidrUtils::toCidrString(iface->ifAddr) << " : "
                    << iface->isExternal << std::endl;

            }
        }

    }
    FwRuleSet  fwrules;

    FwConfig::ParseFwRules("etc/fwrules.cfg", fwrules);
*/
    return 0;
}



