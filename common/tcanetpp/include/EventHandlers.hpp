/**  
  * @file EventHandlers.h
  * 
  *    Defines the interfaces for registering events with the 
  *  EventManager.
  * 
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author  tcarland@gmail.com
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
#ifndef _TCANETPP_EVENTHANDLERS_HPP_
#define _TCANETPP_EVENTHANDLERS_HPP_

#include "tcanetpp_types.h"



namespace tcanetpp {


struct EventTimer;
struct EventIO;


/**  This interface provides the callback mechanism for an 'EventTimer' 
  *  (timer event) within the EventManager. 
 **/
class EventTimerHandler {
    
  public:
  
    virtual ~EventTimerHandler() {}
  
    virtual void timeout  ( const EventTimer & timer ) = 0;
    virtual void finished ( const EventTimer & timer ) = 0;
};



/**  This interface provides the callback mechanism for an 'EventIO' 
  *  (io event) within the EventManager. 
 **/
class EventIOHandler {
    
  public:

    virtual ~EventIOHandler() {}

    virtual void handle_accept  ( const EventIO & io ) = 0;
    virtual void handle_read    ( const EventIO & io ) = 0;
    virtual void handle_write   ( const EventIO & io ) = 0;
    virtual void handle_shut    ( const EventIO & io ) = 0;
    virtual void handle_close   ( const EventIO & io ) = 0;
    virtual void handle_destroy ( const EventIO & io ) = 0;


    virtual bool readable       ( const EventIO & io ) = 0;
    virtual bool writeable      ( const EventIO & io ) = 0;
    
};


}  // namespace


#endif  // _TCANETPP_EVENTHANDLERS_HPP_

