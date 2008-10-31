#ifndef _TNMSD_CLIENTIOHANDLER_H_
#define _TNMSD_CLIENTIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
using namespace tcanetpp;


namespace tnmsd {

class TnmsTree;


class ClientIOHandler : public EventIOHandler {

  public:

    ClientIOHandler();
    ClientIOHandler ( TnmsTree * tree );

    virtual ~ClientIOHandler();

    virtual void handle_accept  ( const EventIO * io );
    virtual void handle_read    ( const EventIO * io );
    virtual void handle_write   ( const EventIO * io );
    virtual void handle_shut    ( const EventIO * io );
    virtual void handle_close   ( const EventIO * io );
    virtual void handle_destroy ( const EventIO * io );
    

    virtual bool readable       ( const EventIO * io );
    virtual bool writeable      ( const EventIO * io );

    void         timeout        ( const EventTimer * timer );


  protected:

    TnmsTree *               _tree;
    std::set<TnmsClient*>    _clients;

};

}  // namespace 

#endif //  _TNMSD_CLIENTIOHANDLER_H_

