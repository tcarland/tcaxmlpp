/** @file
  * 
  * EventManager
  *   Class for handling multiplexing data I/O and timer events.
  *
  * Copyright (c) 2004 Timothy Charlton Arland 
  * @author tca@charltontechnology.net
  *
  * @section LICENSE
  *
  * This file is part of tcanetpp.
  *
  * tcanetpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcanetpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcanetpp.  
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#ifndef _TCANETPP_EVENTMANAGER_H_
#define _TCANETPP_EVENTMANAGER_H_

#include <map>
#include <set>
#include <string>

#include "tcanetpp_types.h"
#include "EventHandlers.hpp"
#include "Socket.h"


namespace tcanetpp {


// macros for normalizing event time units
#define sectoevu(x)  (x * 1000000)
#define msectoevu(x) (x * 1000)
#define evutosec(x)  (x / 1000000)
#define evutomsec(x) (x / 1000)


#define DEFAULT_EVU 500000   // default select timeout
#define MAX_FDVAL   1024000  // max file descriptors
#define NO_EVID     0 


typedef uint64_t  evid_t;    // event registration id


class EventManager;


/**  A timer event within the EventManager is tracked by an instance of this
  *  object. When the event fires, the handler associated with this event is 
  *  provided a const pointer to this struct.
 **/
struct EventTimer {
    evid_t              evid;      // event id

    EventManager*       evmgr;     // evmgr owning this event 
    EventTimerHandler*  handler;   // event handler for this event.

    uint32_t            count;     // number of times to fire event (0 == forever)
    uint32_t            fired;     // number of times event has fired
    long                evsec;     // event interval in seconds (if applicable).
    long                evusec;    // event interval in microseconds (if applicable).
    struct timeval      abstime;   // absolute timeval used to track event time
    bool                absolute;  // boolean indicating a single shot event.
    bool                enabled;   // boolean indicating whether event is active.
     
    EventTimer()
        : evid(0),   evmgr(NULL), handler(NULL), 
          count(0),  fired(0),    evsec(0), 
          evusec(0), absolute(false), 
          enabled(false) 
    {}

    bool operator== ( const EventTimer & timer );
    bool operator<  ( const EventTimer & timer );
};

typedef std::map<evid_t, EventTimer>  EventTimerMap;



/**  The EventIO struct represents an IO event. When an event is fired,
  *  a const pointer to this struct is provided to the associated io 
  *  event handler. ( see EventHandlers.h )
 **/
struct EventIO {
    evid_t             evid;      // event id

    EventManager*      evmgr;     // evmgr owning this event (ie. this)
    EventIOHandler*    handler;   // event handler for this event.

    sockfd_t           sfd;       // event socket id
    void*              rock;      // event object
    struct timeval     abstime;   // time of event firing.
    bool               enabled;   // boolean indicating whether event is valid.
    bool               isServer;  // IO socket event is a server socket.

    EventIO() 
        : evid(0),    evmgr(NULL),    handler(NULL), 
          rock(NULL), enabled(false), isServer(false)
    {
        Socket::ResetDescriptor(sfd);
    }

    bool operator== ( const EventIO & io );
};

typedef std::map<evid_t, EventIO>  EventIOMap;



/**  The EventManager class provides an interface to using select for a
  *  variety of I/O and timer events.  By default the main loop will
  *  run endlessly until setAlarm() is called or if 'dieoff' is set true, 
  *  the event loop will exit once there are no remaining active events in
  *  the system. 
 **/
class EventManager {

  public:

    EventManager ( bool dieoff = false );

    virtual ~EventManager();


    void                 eventLoop();


    evid_t               addTimerEvent  ( EventTimerHandler * handler, 
                                          uint32_t            sec,
                                          uint32_t            usec,
                                          int                 count = 0 );

    evid_t               addTimerEvent  ( EventTimerHandler * handler, 
                                          time_t abstime );

    evid_t               addIOEvent     ( EventIOHandler    * handler,
                                          const sockfd_t    & sfd,
                                          void              * rock     = NULL,
                                          bool                isServer = false );

    bool                 removeEvent    ( const evid_t  & id );

    const EventTimer*    findTimerEvent ( const evid_t  & id ) const;
    const EventIO*       findIOEvent    ( const evid_t  & id ) const;

    bool                 isActive       ( const evid_t  & id ) const;
    bool                 isValidEvent   ( const evid_t  & id ) const;

    size_t               activeEvents()  const;
    size_t               activeTimers()  const;
    size_t               activeClients() const;

    void                 clearTimers();
    void                 clearStaleEvents();

    void                 setDebug       ( bool d );
    void                 setAlarm();

    const std::string&   getErrorStr() const { return this->_errstr; }

 
  public:

    static int           GetTimeOfDay   ( timeval * t );

    static float         TimevalToMs    ( const timeval * tv );

    static float         TimevalDiffMs  ( const timeval * t2, 
                                          const timeval * t1 );

    static void          TimevalDiff    ( const timeval * t2, 
                                          const timeval * t1, 
                                          timeval * result );


  protected:

    void                 verifyTimers();
    void                 checkTimers    ( const timeval    & now );
    void                 checkMinTimer  ( const EventTimer & timer );

    void                 destroyEvent   ( EventTimer & timer );
    void                 destroyEvent   ( EventIO    & io );

    evid_t               getNewEventId();


  protected:

    typedef std::set<evid_t>   EventSet;

    EventSet                  _events;
    EventTimerMap             _timers;
    EventIOMap                _clients;


    evid_t                    _evid, _lastid;
    fd_set                    _rset, _wset, _xset;
    long                      _minevu;

    std::string               _errstr;
    bool                      _dieoff;
    bool                      _alarm;
    bool                      _debug;

    static int                _maxfdp;
 
};

}  // namespace


#endif  // _TCANETPP_EVENTMANAGER_H_

