#ifndef fwgen_h
#define fwgen_h

#include <list>
#include <set>
#include <map>
#include <string>

#include "Prefix.hpp"
using namespace tcanetpp;


namespace fwgen {


enum FwType
{
    IPTABLES = 0,
    CISCO_ACL = 1,
    CISCO_FW  = 2,
    BSD = 3
};


typedef struct FwInterface {
    std::string         ifName;
    Prefix              ifAddr;
    bool                isExternal;
} FwIface;

typedef std::list< FwIface* > FwIfaceList;


struct FwDevice {
    std::string    deviceName;
    FwIfaceList    ifaceList;
    FwType         fwType;

    FwDevice ( const std::string & devname )
        : deviceName(devname) {}
};

typedef std::list< FwDevice* > FwDevList;


} // namespace

#endif
