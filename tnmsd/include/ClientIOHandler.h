#ifndef _TNMSD_CLIENTIOHANDLER_H_
#define _TNMSD_CLIENTIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
#include "Exception.hpp"
using namespace tcanetpp;

#include "TnmsTree.h"
#include "TnmsClient.h"
using namespace tnmsCore;

namespace tnmsd {


class ClientIOHandler : public EventIOHandler {

  public:

    ClientIOHandler ( TnmsTree * tree ) throw ( Exception );

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

