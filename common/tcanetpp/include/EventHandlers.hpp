/**  EventHandlers
  * 
  *    Contains handlers for registering events with the EventManager
  *
  *  @Author   tcarland@gmail.com
  *  @Version  5.0
 **/
#ifndef _TCANETPP_EVENTHANDLERS_HPP_
#define _TCANETPP_EVENTHANDLERS_HPP_

#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif


namespace tcanetpp {

struct EventTimer;
struct EventIO;
class Socket;



class TimerHandler {
    
  public:
  
    virtual ~TimerHandler() {}
  
    virtual void timeout  ( const EventTimer * timer ) = 0;
    virtual void finished ( const EventTimer * timer ) {}
};



class IOHandler {
    
  public:

    virtual ~IOHandler() {}

    virtual void handle_accept  ( const EventIO * io ) {}
    virtual void handle_read    ( const EventIO * io ) {}
    virtual void handle_write   ( const EventIO * io ) {}
    virtual void handle_shut    ( const EventIO * io ) {}
    virtual void handle_close   ( const EventIO * io ) {}
    virtual void handle_destroy ( const EventIO * io ) {}
    

    virtual bool readable       ( const EventIO * io ) = 0;
    virtual bool writeable      ( const EventIO * io ) = 0;
    
};


}  // namespace


#endif  // _TCANETPP_EVENTHANDLERS_HPP_

