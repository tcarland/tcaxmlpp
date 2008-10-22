#ifndef fwgen_h
#define fwgen_h

#include <list>
#include <set>
#include <map>
#include <string>

#include "Prefix.hpp"
using namespace tcanetpp;


namespace fwgen {


#define FWGEN_PROTOCOLS_FILE  "/etc/protocols"


typedef enum eFwType
{
    IPTABLES  = 0,
    CISCO_ACL = 1,
    CISCO_FW  = 2,
    BSD       = 3
} efwtype_t;


/**  A hard-coded protocol mapping for use only if unable to 
 *   retrieve protocol mappings from another source (ie. /etc/protocols)
 **/
typedef enum eFwProtocol 
{
    FWPROTO_IP    = 0,
    FWPROTO_ICMP  = 1,
    FWPROTO_IGMP  = 2,
    FWPROTO_TCP   = 6,
    FWPROTO_UDP   = 17,
    FWPROTO_RDP   = 27,
    FWPROTO_RSVP  = 46,
    FWPROTO_GRE   = 47,
    FWPROTO_ESP   = 50,
    FWPROTO_RSPF  = 73,
    FWPROTO_OSPF  = 89,
    FWPROTO_IPIP  = 94,
    FWPROTO_ENCAP = 98,
    FWPROTO_PIM   = 103
} efwproto_t;

typedef std::map< std::string, uint16_t >   FwProtoMap;


typedef struct FwDevicePort
{
    std::string     portName;
    Prefix          portAddr;
    bool            isExternal;

    FwDevicePort ( const std::string & portname = "" )
        : portName(portname)
    {}
} fwdevport_t;

typedef std::list< FwDevicePort >               FwPortList;
typedef std::map< std::string, FwDevicePort >   FwPortMap;



struct FwDevice
{
    std::string     deviceName;
    FwPortMap       portsmap;
    eFwType         fwType;

    FwDevice ( const std::string & devicename = "" )
        : deviceName(devicename) {}
};

typedef std::list< FwDevice >               FwDeviceList;
typedef std::map< std::string, FwDevice >   FwDeviceMap;




struct FwPort 
{
    uint16_t    port;
    uint16_t    port_high;
    bool        ranged;

    FwPort()
        : port(0),
          port_high(0),
          ranged(false)
    {}
};


struct FwRule 
{
    bool        permit;
    Prefix      src;
    FwPort      srcport;
    Prefix      dst;
    FwPort      dstport;
    uint16_t    proto;
    bool        established;

    FwRule()
        : permit(false),
          proto(FWPROTO_IP),
          established(false)
    {}

};

typedef std::list<FwRule>  FwRuleList;



} // namespace

#endif // fwgen_h
