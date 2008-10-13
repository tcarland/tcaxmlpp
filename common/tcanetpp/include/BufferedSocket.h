#ifndef BUFFEREDSOCKET_H_
#define BUFFEREDSOCKET_H_



#include "Socket.h"
#include "CircularBuffer.h"


namespace tcanetpp {



class BufferedSocket : public Socket {

  public:

    /**  Socket factory class for handling Socket::accept()
     *   in derived classes */
    class BufferedSocketFactory : public Socket::SocketFactory {
      public:
        virtual ~BufferedSocketFactory() {}

        virtual Socket* operator() ( sockfd_t           & fd, 
                                     struct sockaddr_in & csock )
        {
            return( (Socket*) new BufferedSocket(fd, csock) );
        }
    };

    static BufferedSocketFactory   factory;


  protected:

    BufferedSocket ( sockfd_t           & fd, 
                     struct sockaddr_in & csock );

  public:

    BufferedSocket();

    BufferedSocket ( ipv4addr_t  ip, 
                     uint16_t    port, 
                     SocketType  socktype, 
                     int         proto );

    virtual ~BufferedSocket();


    /*  from Socket */
    
    virtual int      init  ( bool block = false );
    virtual ssize_t  read  ( void * vptr, size_t n );
    virtual ssize_t  write ( const void * vptr, size_t n );
    
    /*  BufferedSocket methods  */

    virtual ssize_t  flush(); 
    virtual void     clear();
    virtual void     close();
    
    virtual size_t   dataAvailable();
    virtual size_t   flushAvailable();
    
    virtual size_t   reverse ( size_t offset );
    virtual size_t   skip    ( size_t offset );

    /*  Get/Set methods for buffer sizes  */
    void             rxBufferSize ( size_t sz );
    size_t           rxBufferSize();
    
    void             txBufferSize ( size_t sz );
    size_t           txBufferSize();

    void             enableTxBuffer();

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


#endif // BUFFEREDSOCKET_H_

