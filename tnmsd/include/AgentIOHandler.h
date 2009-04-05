#ifndef _TNMSD_AGENTIOHANDLER_H_
#define _TNMSD_AGENTIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
#include "Exception.hpp"
using namespace tcanetpp;


namespace tnmsd {

class TnmsTree;
class TnmsClient;

typedef std::set<TnmsClient*>  ClientSet;


class AgentIOHandler : public EventIOHandler {

  public:

    AgentIOHandler ( TnmsTree * tree ) throw ( Exception );

    virtual ~AgentIOHandler();

    void timeout ( const EventTimer * timer );
    void addMirrorConnection ( TnmsClient * client );

    virtual void handle_accept  ( const EventIO * io );
    virtual void handle_read    ( const EventIO * io );
    virtual void handle_write   ( const EventIO * io );
    virtual void handle_shut    ( const EventIO * io );
    virtual void handle_close   ( const EventIO * io );
    virtual void handle_destroy ( const EventIO * io );
    

    virtual bool readable       ( const EventIO * io );
    virtual bool writeable      ( const EventIO * io );

  protected:

    TnmsTree *  _tree;
    //auth

    std::set<TnmsClient*>       _clients;
    std::set<TnmsClient*>       _mirrors;

};

}  // namespace 

#endif //  _TNMSD_AGENTIOHANDLER_H_

