#ifndef _TNMSAUTH_AUTHIOHANDLER_H_
#define _TNMSAUTH_AUTHIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
using namespace tcanetpp;


namespace tnmsauth {


class TnmsAuthThread;
class TnmsAuthClient;

typedef std::set<TnmsAuthClient*>  ClientSet;



class AuthIOHandler : public EventIOHandler {

  public:

    AuthIOHandler ( TnmsAuthThread * authThread = NULL );
    virtual ~AuthIOHandler();

    /*  EventIOHandler */

    virtual void   handle_accept      ( const EventIO * io );
    virtual void   handle_destroy     ( const EventIO * io );
    
    virtual bool   readable           ( const EventIO * io );

    /*  AuthIOHandler */

    void           timeout            ( const time_t & now );


  protected:

    TnmsAuthThread*     _authThread;
    ClientSet           _clients;

};


} // namespace

#endif  // _TNMSAUTH_AUTHIOHANDLER_H_

