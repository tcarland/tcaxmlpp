#ifndef _TNMSCLIENT_CLIENTIOHANDLER_H_
#define _TNMSCLIENT_CLIENTIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
#include "Exception.hpp"
using namespace tcanetpp;

#include "TnmsClient.h"
using namespace tnmsCore;


namespace tnmsClient {

class ClientTreeMutex;


typedef std::set<TnmsClient*>   ClientSet;


class ClientIOHandler : public tcanetpp::EventIOHandler {

  public:

    ClientIOHandler ( ClientTreeMutex * tree );

    virtual ~ClientIOHandler();
    

    /* EventIOHandler */

    virtual void handle_accept  ( const EventIO & io ) {}
    virtual void handle_read    ( const EventIO & io );
    virtual void handle_write   ( const EventIO & io );
    virtual void handle_shut    ( const EventIO & io ) {}
    virtual void handle_close   ( const EventIO & io );
    virtual void handle_destroy ( const EventIO & io );

    virtual bool readable       ( const EventIO & io );
    virtual bool writeable      ( const EventIO & io );

    /* ClientIOHandler */

    void         timeout        ( const EventTimer & timer );

    void         addClient      ( TnmsClient * client );
    void         removeClient   ( TnmsClient * client );


  protected:

    ClientSet           _clients;
    ClientTreeMutex *   _mtree;

};

} // namespace

#endif  //  _TNMSCLIENT_CLIENTIOHANDLER_H_

