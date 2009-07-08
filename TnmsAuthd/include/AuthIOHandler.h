#ifndef _TNMSAUTH_AUTHIOHANDLER_H_
#define _TNMSAUTH_AUTHIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
using namespace tcanetpp;


namespace tnmsauth {


class AuthDbThread;
class AuthdClient;



typedef std::set<AuthdClient*>  ClientSet;



class AuthIOHandler : public EventIOHandler {

  public:

    AuthIOHandler ( AuthDbThread * authdb = NULL );
    virtual ~AuthIOHandler();

    /*  EventIOHandler */

    virtual void   handle_accept      ( const EventIO * io );
    virtual void   handle_destroy     ( const EventIO * io );
    
    virtual bool   readable           ( const EventIO * io );
    virtual bool   writeable          ( const EventIO * io );

    /*  AuthIOHandler */

    void           timeout            ( const time_t & now );


  protected:

    AuthDbThread*       _authDb;
    ClientSet           _clients;

};


} // namespace

#endif  // _TNMSAUTH_AUTHIOHANDLER_H_

