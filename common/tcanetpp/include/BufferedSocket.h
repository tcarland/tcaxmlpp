/** 
  * @file BufferedSocket.h
  *
  * Copyright (c) 2002 Timothy Charlton Arland 
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
#ifndef _TCANETPP_BUFFEREDSOCKET_H_
#define _TCANETPP_BUFFEREDSOCKET_H_


#include "Socket.h"
#include "CircularBuffer.h"


namespace tcanetpp {


/**  BufferedSocket adds read and optional write buffering
  *  functionality to the base tcanetpp::Socket class.
 **/
class BufferedSocket : public Socket {

  public:

    /**  Socket factory class for handling Socket::accept()
     *   in derived classes */
    class BufferedSocketFactory : public Socket::SocketFactory {
      public:
        virtual ~BufferedSocketFactory() {}

        virtual Socket* operator() ( sockfd_t         & fd,
                                     sockaddr_storage & csock,
                                     SocketType         type,
                                     int                proto );
    };

    static BufferedSocketFactory   factory;


  public:

    BufferedSocket();

    BufferedSocket ( ipv4addr_t   ip,
                     uint16_t     port,
                     SocketType   socktype,
                     int          proto ) throw ( SocketException );

    BufferedSocket ( ipv6addr_t   ip,
                     uint16_t     port,
                     SocketType   socktype,
                     int          proto ) throw ( SocketException );

    BufferedSocket ( sockaddr_t * sa,
                     size_t       salen,
                     uint16_t     port,
                     SocketType   type,
                     int          protocol ) throw ( SocketException );

    virtual ~BufferedSocket();


  protected:

    BufferedSocket ( sockfd_t   & fd,
                     sockaddr_t & csock,
                     SocketType   type,
                     int          proto );

  public:

    /*  from Socket */
    virtual int      init  ( bool block = false );
    virtual ssize_t  read  ( void * vptr, size_t n );
    virtual ssize_t  write ( const void * vptr, size_t n );
    
    /*  BufferedSocket methods  */

    virtual ssize_t  flush(); 
    virtual void     clear();
    virtual void     close();
    
    virtual size_t   dataAvailable();
    size_t           readAvailable() { return this->dataAvailable(); }
    virtual size_t   flushAvailable();
    size_t           writeAvailable() { return this->flushAvailable(); }
    
    virtual size_t   reverse ( size_t offset );
    virtual size_t   skip    ( size_t offset );

    /*  Get/Set methods for buffer sizes  */
    void             rxBufferSize ( size_t sz );
    size_t           rxBufferSize();
    
    void             txBufferSize ( size_t sz );
    size_t           txBufferSize();

    void             enableTxBuffer();
    void             disableTxBuffer();

    CircularBuffer*  getRxBuffer() { return _rbuffer; }
    CircularBuffer*  getTxBuffer() { return _wbuffer; }


  protected:

    virtual ssize_t  bufferedWrite ( const void * vptr, size_t n );
    virtual ssize_t  bufferData();

    
  protected:

    CircularBuffer*     _rbuffer;
    CircularBuffer*     _wbuffer;

    bool                _wbx;

};


}  // namespace


#endif // _TCANETPP_BUFFEREDSOCKET_H_

