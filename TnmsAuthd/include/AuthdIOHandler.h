#ifndef _TNMSAUTH_AUTHDIOHANDLER_H_
#define _TNMSAUTH_AUTHDIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
#include "EventManager.h"
using namespace tcanetpp;


namespace tnmsauth {


class AuthDbThread;
class AuthdClient;



typedef std::set<AuthdClient*>  ClientSet;



class AuthdIOHandler : public EventIOHandler {

  public:

    AuthdIOHandler ( AuthDbThread * authdb = NULL );
    virtual ~AuthdIOHandler();

    /*  EventIOHandler */

    virtual void   handle_accept      ( const EventIO * io );
    virtual void   handle_read        ( const EventIO * io );
    virtual void   handle_write       ( const EventIO * io );
    virtual void   handle_shut        ( const EventIO * io ) {}
    virtual void   handle_close       ( const EventIO * io );
    virtual void   handle_destroy     ( const EventIO * io );
    
    virtual bool   readable           ( const EventIO * io );
    virtual bool   writeable          ( const EventIO * io );

    /*  AuthdIOHandler */

    void           timeout            ( const time_t & now );


  protected:

    AuthDbThread*       _authDb;
    ClientSet           _clients;

};


} // namespace

#endif  // _TNMSAUTH_AUTHDIOHANDLER_H_

