#ifndef _TNMSCLIENTIOHANDLER_H_
#define _TNMSCLIENTIOHANDLER_H_

#include <set>


#include "EventHandlers.hpp"
#include "Exception.hpp"
#include "ThreadLock.h"
using namespace tcanetpp;

#include "TnmsClient.h"
using namespace tnmsCore;


typedef std::set<TnmsClient*>   ClientSet;


class TnmsClientIOHandler : public tcanetpp::EventIOHandler {

  public:

    TnmsClientIOHandler ( ThreadLock * readLock = NULL );
    virtual ~TnmsClientIOHandler();
    
    /* EventIOHandler */

    virtual void handle_read    ( const EventIO * io );
    virtual void handle_write   ( const EventIO * io );
    virtual void handle_close   ( const EventIO * io );
    virtual void handle_destroy ( const EventIO * io );

    virtual bool readable       ( const EventIO * io );
    virtual bool writeable      ( const EventIO * io );

    /* ClientIOHandler */

    void         timeout        ( const EventTimer * timer );

    void         addClient      ( TnmsClient * client );
    void         removeClient   ( TnmsClient * client );

  protected:

    ClientSet    _clients;
    ThreadLock * _rlock;

};


#endif  //  _TNMSCLIENTIOHANDLER_H_

