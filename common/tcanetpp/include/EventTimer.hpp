/**
  * @file EventTimer.hpp
  *
  *     EventTimer represents a timer based event within the
  *   EventManager.
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland
  * @author  tca@charltontechnology.net
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
#ifndef _TCANETPP_EVENTTIMER_H_
#define _TCANETPP_EVENTTIMER_H_


#include "tcanetpp_types.h"


namespace tcanetpp {


class EventManager;
class EventTimerHandler;


/**  A timer event within the EventManager is tracked by an instance of 
  *  this object. When the event fires, the event handler invoked is 
  *  provided a reference to this object.
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

    bool operator== ( const EventTimer & timer )
    {
        return(this->evid == timer.evid);
    }

    bool operator<  ( const EventTimer & timer )
    {
        if ( this->evsec > 0 && this->evsec != timer.evsec )
            return( this->evsec < timer.evsec );
        return( this->evusec < timer.evusec );
    }

};


}  // namespace

#endif  // _TCANETPP_EVENTTIMER_H_

