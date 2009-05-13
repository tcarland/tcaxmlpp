#ifndef _TNMSCORE_AUTHIOHANDLER_H_
#define _TNMSCORE_AUTHIOHANDLER_H_


#include "EventHandlers.hpp"
using namespace tcanetpp;

namespace tnmsCore {


class AuthIOHandler : public EventIOHandler {

  public:

    AuthIOHandler() {}
    virtual ~AuthIOHandler() {}


    void        timeout         ( const EventTimer * timer );

    void        handle_accept   ( const EventIO * io );
    void        handle_read     ( const EventIO * io );
    void        handle_write    ( const EventIO * io );
    void        handle_shut     ( const EventIO * io );
    void        handle_close    ( const EventIO * io );
    void        handle_destroy  ( const EventIO * io );
    bool        readable        ( const EventIO * io );
    bool        writeable       ( const EventIO * io );

};

}  // namespace


#endif  // _TNMSCORE_AUTHIOHANDLER_H_
