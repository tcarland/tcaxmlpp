#ifndef _TNMSCORE_AUTHIOHANDLER_H_
#define _TNMSCORE_AUTHIOHANDLER_H_


#include "EventHandlers.hpp"
using namespace tcanetpp;

namespace tnmsCore {


class AuthIOHandler : public EventIOHandler {

  public:

    AuthIOHandler() {}

    virtual ~AuthIOHandler() {}

    virtual void        handle_accept   ( const EventIO * io ) {}
    virtual void        handle_read     ( const EventIO * io );
    virtual void        handle_write    ( const EventIO * io );
    virtual void        handle_close    ( const EventIO * io );
    virtual void        handle_destroy  ( const EventIO * io ) {}

    virtual bool        readable        ( const EventIO * io );
    virtual bool        writeable       ( const EventIO * io );

};

}  // namespace


#endif  // _TNMSCORE_AUTHIOHANDLER_H_
