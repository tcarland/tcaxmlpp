#ifndef _TNMSD_CLIENTSTATS_HPP_
#define _TNMSD_CLIENTSTATS_HPP_

#include "TnmsMetric.h"
using namespace tnmsCore;


namespace tnmsd {


#define AGENT_SUBNAME   "Agents"
#define CLIENT_SUBNAME  "Clients"
#define MIRROR_SUBNAME  "Mirrors"
#define RECEIVE_NAME    "Received"
#define TRANSMIT_NAME   "Sent"
#define LASTCONN_NAME   "LastConnected"


struct ClientStats {

    std::string       name;   // root client metric name

    TnmsMetric        connState;
    TnmsMetric        lastConn;
    TnmsMetric        rxCtr;
    TnmsMetric        txCtr;

};

typedef struct ClientStats  ClientStat;


}  // namespace

#endif  // _TNMSD_CLIENTSTATS_HPP_

