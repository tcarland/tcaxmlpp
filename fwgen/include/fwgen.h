#ifndef fwgen_h
#define fwgen_h

#include <list>
#include <set>
#include <map>
#include <string>

#include "Prefix.hpp"
using namespace tcanetpp;


namespace fwgen {


typedef enum eFwType
{
    IPTABLES = 0,
    CISCO_ACL = 1,
    CISCO_FW  = 2,
    BSD = 3
} fwtype_t;


typedef struct FwDevicePort
{
    std::string     ifName;
    Prefix          ifAddr;
    bool            isExternal;
} fwdevport_t;


typedef std::list< FwDevicePort >  FwPortList;


struct FwDevice
{
    std::string     deviceName;
    FwPortList      portslist;
    eFwType         fwType;

    FwDevice ( const std::string & devicename )
        : deviceName(devicename) {}
};

typedef std::list< FwDevice > FwDeviceList;


struct FwZone
{
    std::string     zoneName;
    FwDeviceList    deviceList;

    FwZone ( const std::string & zonename = "" )
        : zoneName(zonename) {}
};


} // namespace

#endif // fwgen_h
