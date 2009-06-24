#ifndef _TNMSD_CLIENTIOHANDLER_H_
#define _TNMSD_CLIENTIOHANDLER_H_

#include <set>
#include <map>

#include "EventHandlers.hpp"
#include "Exception.hpp"
using namespace tcanetpp;

#include "TnmsClient.h"
using namespace tnmsCore;


namespace tnmsd {


typedef std::set<TnmsClient*>               ClientSet;


class ClientIOHandler : public EventIOHandler {

  public:

    ClientIOHandler ( TnmsTree * tree, AuthClient * auth = NULL )
        throw ( Exception );

    virtual ~ClientIOHandler();

    /* EventIOHandler */

    virtual void handle_accept  ( const EventIO * io );
    virtual void handle_read    ( const EventIO * io );
    virtual void handle_write   ( const EventIO * io );
    virtual void handle_shut    ( const EventIO * io );
    virtual void handle_close   ( const EventIO * io );
    virtual void handle_destroy ( const EventIO * io );

    virtual bool readable       ( const EventIO * io );
    virtual bool writeable      ( const EventIO * io );

    /* ClientIOHandler */

    void         timeout        ( const EventTimer * timer );

    void         addMirror      ( TnmsClient * client );
    void         eraseMirror    ( TnmsClient * client );


  protected:

    TnmsTree *               _tree;
    AuthClient *             _auth;

    ClientSet                _clients;

};

}  // namespace 

#endif //  _TNMSD_CLIENTIOHANDLER_H_

