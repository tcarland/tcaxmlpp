#ifndef _INCLUDE_POLLERQUERY_H_
#define _INCLUDE_POLLERQUERY_H_

#include "SnmpOid.h"
using namespace tcasnmp;


namespace pollap {


#define DEFAULT_REFRESH_INTERVAL  300
#define DEFAULT_UPDATE_INTERVAL   60

#define ELEMENT_ROOT "pollap/"



enum SnmpQueryType 
{
    SNMPQUERY_TYPE_NONE,
    SNMPQUERY_TYPE_GET,
    SNMPQUERY_TYPE_GETNEXT,
};


struct PollerQuery;
struct PollerDeviceQuery;
struct PollerDevice;


typedef std::list<PollerQuery>                    PollerQueryList;
typedef std::map<std::string, PollerQuery>        PollerQueryMap;

typedef std::list<PollerDevice>                   PollerDeviceList;
typedef std::map<std::string, PollerDeviceQuery>  DeviceQueryMap;

typedef std::map<int, std::string>                PollerResultMap;




struct PollerQuery {
    std::string    name;
    std::string    key;
    std::string    desc;

    SnmpOid        oid;

    bool           oid_is_key;
    bool           use_slot_num;

    uint32_t       query_type;
    time_t         refresh;

    PollerQueryList  subQueries;
    PollerResultMap  resultMap;

    PollerQuery()
        : oid_is_key(false), 
          use_slot_num(false),
          query_type(0), 
          refresh(DEFAULT_REFRESH_INTERVAL)
    {}
};


struct PollerDeviceQuery {
    std::string     query_name;
    long            slot_number;
    time_t          update_interval;
    time_t          last_query;

    PollerDeviceQuery() 
        : slot_number(0),
          update_interval(DEFAULT_UPDATE_INTERVAL),
          last_query(0)
    {}
};


struct PollerDevice {
    std::string     name;
    std::string     host;
    std::string     community;

    DeviceQueryMap  queryMap;

    PollerDevice() {}
};



} // namespace

#endif  // _INCLUDE_SNMPQUERY_H_
