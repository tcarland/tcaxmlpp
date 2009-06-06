#ifndef _TNMSAUTH_AUTHIOHANDLER_H_
#define _TNMSAUTH_AUTHIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
using namespace tcanetpp;

#include "SoapClient.h"

namespace tnmsauth {

typedef std::set<SoapClient*>  SoapClientSet;


class AuthIOHandler : public EventIOHandler {

  public:

    AuthIOHandler() {}
    ~AuthIOHandler() {}

    void           timeout            ( const time_t & now );

    void           event_timeout_secs ( time_t  secs );
    const time_t&  event_timeout_secs() const;

    void           max_connections    ( int  max );
    int            max_connections() const;

    /*  EventIOHandler */
    virtual void   handle_accept      ( const EventIO * io );
    virtual void   handle_destroy     ( const EventIO * io );
    
    virtual bool   readable           ( const EventIO * io );


};


} // namespace

#endif  // _TNMSAUTH_AUTHIOHANDLER_H_

