#ifndef _TNMSAUTH_SOAPIOHANDLER_H_
#define _TNMSAUTH_SOAPIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
using namespace tcanetpp;

#include "SoapClient.h"

namespace tnmsauth {

typedef std::set<SoapThread*>  SoapThreadList;


class SoapIOHandler : public EventIOHandler {

  public:

    SoapIOHandler() {}

    virtual ~SoapIOHandler() {}

    void           timeout            ( const time_t & now );

    void           event_timeout_secs ( time_t  secs );
    const time_t&  event_timeout_secs() const;

    void           max_connections    ( int  max );
    int            max_connections() const;

    /*  EventIOHandler */
    virtual void   handle_accept      ( const EventIO * io );
    virtual void   handle_destroy     ( const EventIO * io );
    
    virtual bool   readable           ( const EventIO * io );

  protected:

    SoapThreadList   _sthreads;

    int              _max_threads;
    int              _thread_count;
    time_t           _thread_timeout;

};


} // namespace

#endif  // _TNMSAUTH_SOAPIOHANDLER_H_

