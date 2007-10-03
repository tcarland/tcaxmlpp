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



/**  The TimerHandler interface for the EventManager. 
  *  A derived instance of this class can be 
  *  used to define a timer event. For example:
  *
  *    class MyTimer : public tcanetpp::TimerHandler 
  *    {
  *      public:
  *        virtual ~MyTimer() {}
  *
  *        void timeout ( const tcanetpp::EventTimer * timer )
  *        {
  *            doWork();
  *        }
  *    };
  *
  *    EXAMPLE: {
  *        tcanetpp::EventManager evmgr;
  *        tcanetpp::MyTimer      timer;
  *
  *        evmgr.addTimerEvent(timer, 5, 0);
  *        evmgr.eventLoop();
  *    }
  *
  *    This will result in the MyTimer::timeout() function being called every 
  *    5 seconds.
 **/
class TimerHandler {
    
  public:
  
    virtual ~TimerHandler() {}
  
    virtual void timeout  ( const EventTimer * timer ) = 0;
    virtual void finished ( const EventTimer * timer ) {}
};



/**  The IOHandler class provides an interface for managing IO events from the 
  *  EventManager class.  This class should be derived by an object providing 
  *  the read/write implementation for a particular set of IO based operations. 
  *  Any pollable file descriptor will work.
 **/
class IOHandler {
    
  public:

    virtual ~IOHandler() {}

    virtual void handle_accept  ( const EventIO * io ) {}
    virtual void handle_read    ( const EventIO * io ) {}
    virtual void handle_write   ( const EventIO * io ) {}
    virtual void handle_shut    ( const EventIO * io ) {}
    virtual void handle_close   ( const EventIO * io ) {}
    virtual void handle_destroy ( const EventIO * io ) {}
    

    /**  This method must be implemented to return whether the IOHandler in 
      *  question should be considered readable and as a result will be added 
      *  to the selects read set. Once set readable, handle_read will be called 
      *  each loop that select determines the underlying file descriptor as
      *  readable. This is often not a conditional situation as most sockets
      *  should always be readable, so the provided implementation is often as
      *  simple as follows: 
      *
      *    bool readable ( const EventIO * io ) { return true; }
     **/
    virtual bool readable       ( const EventIO * io ) = 0;

    /**  This method must be implemented to return whether the IOHandler in 
      *  question should be considered writeable.  Unlike 'readable()', this 
      *  is usually a conditional situation since descriptors are usually  
      *  writeable and as such, if added to the select's write set, would result 
      *  in select racing on the descriptors. Generally the implementation
      *  would return true or false based on whether there is data available 
      *  waiting to be written. If the EventManager is racing, look here first.
     **/
    virtual bool writeable      ( const EventIO * io ) = 0;
    
};


}  // namespace


#endif  // _TCANETPP_EVENTHANDLERS_HPP_

