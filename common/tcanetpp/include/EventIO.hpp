/**
  * @file EventIO.hpp
  *
  *     EventIO represents an I/O event (eg. Socket descriptor)
  *   for the EventManager.
  *
  * Copyright (c) 2002,2008-2012 Timothy Charlton Arland
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
#ifndef _TCANETPP_EVENTIO_H_
#define _TCANETPP_EVENTIO_H_


#include "tcanetpp_types.h"


namespace tcanetpp {

class EventManager;
class EventIOHandler;



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
        Socket::ResetDescriptor(this->sfd);
    }

    bool operator== ( const EventIO & io )
    {
        return( this->evid == io.evid );
    }
};

}  // namespace


#endif  // _TCANETPP_EVENTIO_H_
