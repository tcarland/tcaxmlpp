#ifndef _TNMSD_AGENTIOHANDLER_H_
#define _TNMSD_AGENTIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
#include "Exception.hpp"
using namespace tcanetpp;

#include "TnmsClient.h"
using namespace tnmsCore;

#include "ClientStats.hpp"


namespace tnmsd {


typedef std::set<TnmsClient*>              ClientSet;
typedef std::map<TnmsClient*, ClientStat>  ClientMap;


class AgentIOHandler : public EventIOHandler {

  public:

    AgentIOHandler ( TnmsTree * tree, AuthClient * auth = NULL ) 
        throw ( Exception );

    virtual ~AgentIOHandler();

    /* EventIOHandler */

    virtual void handle_accept  ( const EventIO * io );
    virtual void handle_read    ( const EventIO * io );
    virtual void handle_write   ( const EventIO * io );
    virtual void handle_shut    ( const EventIO * io );
    virtual void handle_close   ( const EventIO * io );
    virtual void handle_destroy ( const EventIO * io );
    

    virtual bool readable       ( const EventIO * io );
    virtual bool writeable      ( const EventIO * io );

    /* AgentIOHandler */

    void         timeout        ( const EventTimer * timer );

    /* Statistics */
    
    void         setPrefix      ( const std::string & prefix );

    void         initStat       ( TnmsClient  * client );
    void         updateStat     ( TnmsClient  * client,
                                  ClientStat  & stat );
    void         endStat        ( TnmsClient  * client );

    void         sendStats();

  protected:

    TnmsTree *                  _tree;
    AuthClient *                _auth;

    ClientSet                   _clients;
    ClientMap                   _clMap;

    std::string                 _prefix;
};

}  // namespace 

#endif //  _TNMSD_AGENTIOHANDLER_H_

