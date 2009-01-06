#ifndef _TNMSD_AGENTIOHANDLER_H_
#define _TNMSD_AGENTIOHANDLER_H_


#include "EventHandlers.hpp"
using namespace tcanetpp;


namespace tnmsd {

class TnmsTree;

class AgentIOHandler : public EventIOHandler {

  public:

    AgentIOHandler();
    AgentIOHandler ( TnmsTree * tree );

    virtual ~AgentIOHandler();

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

