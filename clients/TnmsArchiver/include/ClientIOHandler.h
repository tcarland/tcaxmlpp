#ifndef _TNMSDB_CLIENTIOHANDLER_H_
#define _TNMSDB_CLIENTIOHANDLER_H_

#include <set>

#include "EventManager.h"
using namespace tcanetpp;

#include "TnmsClient.h"
using namespace tnmsCore;


namespace tnmsdb {

typedef std::set<TnmsClient*>    ClientSet;


class ClientIOHandler : public EventIOHandler {

  public:

    ClientIOHandler();

    virtual ~ClientIOHandler();

    /* EventIOHandler */

    virtual void handle_accept  ( const EventIO & io );
    virtual void handle_read    ( const EventIO & io );
    virtual void handle_write   ( const EventIO & io );
    virtual void handle_shut    ( const EventIO & io );
    virtual void handle_close   ( const EventIO & io );
    virtual void handle_destroy ( const EventIO & io );

    virtual bool readable       ( const EventIO & io );
    virtual bool writeable      ( const EventIO & io );

    /* ClientIOHandler */

    void         timeout        ( const EventTimer & timer );

    void         addMirror      ( TnmsClient  * client );
    void         eraseMirror    ( TnmsClient  * client );

  protected:

    ClientSet                _clients;
    time_t                   _recon;

};

}  // namespace 

#endif //  _TNMSDB_CLIENTIOHANDLER_H_

