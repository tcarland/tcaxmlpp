/**  BufferedSocket
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.1
 **/
#define _TCANETPP_BUFFEREDSOCKET_CPP_


#include "BufferedSocket.h"


namespace tcanetpp {


static const
char rcsid[] = "$Id: BufferedSocket.cpp,v 0.3 2004/02/03 13:02:00 tca Exp $";



// static factory for accept
BufferedSocket::BufferedSocketFactory  BufferedSocket::factory;


// ----------------------------------------------------------------------

BufferedSocket::BufferedSocket() 
    : Socket(),
      _rbuffer(NULL),
      _wbuffer(NULL),
      _wbx(false) 
{}

BufferedSocket::BufferedSocket ( ipv4addr_t ip, uint16_t port, SocketType socktype, int proto )
    : Socket(ip, port, socktype, proto),
      _rbuffer(new CircularBuffer()),
      _wbuffer(NULL),
      _wbx(false)
{
    this->init(false);
}

BufferedSocket::BufferedSocket ( sockfd_t & fd, struct sockaddr_in & csock )
    : Socket(fd, csock),
      _rbuffer(new CircularBuffer()),
      _wbuffer(NULL),
      _wbx(false)
{
    this->init(false);
}

BufferedSocket::~BufferedSocket()
{
    if ( _rbuffer )
    	delete _rbuffer;
    if ( _wbuffer )
    	delete _wbuffer;
}

// ----------------------------------------------------------------------

int
BufferedSocket::init ( bool block )
{
    int i = Socket::init(block);
    this->setSocketOption(SocketOption::SetRcvBuf(65535));
    return i;
}

// ----------------------------------------------------------------------

ssize_t
BufferedSocket::read ( void * vptr, size_t n )
{
    ssize_t  rt = 0;

    if ( (rt = this->bufferData()) < 0 )
        return rt;

    return _rbuffer->read(vptr, n);
}

// ----------------------------------------------------------------------

ssize_t
BufferedSocket::write ( const void * vptr, size_t n )
{
    return this->bufferedWrite(vptr, n);
}

// ----------------------------------------------------------------------

void
BufferedSocket::clear()
{
    if ( _wbuffer )
        _wbuffer->clear();
    _rbuffer->clear();
}

// ----------------------------------------------------------------------

void
BufferedSocket::close()
{
    Socket::close();
    this->clear();
}

// ----------------------------------------------------------------------

ssize_t
BufferedSocket::flush()
{
    char    *rptr;
    size_t   size;
    ssize_t  wtn, r;

    if ( !_wbx || _wbuffer->readAvailable() == 0 )
        return 1;

    do {
        rptr = _wbuffer->getReadPtr(&size);
        wtn  = this->nwriten(rptr, size);

        if ( wtn < 0 ) {
            r   = wtn;
            wtn = 0;
        } else {
            r   = wtn;
        }

        _wbuffer->setReadPtr(wtn);

    } while ( _wbuffer->readAvailable() > 0 && r > 0 );

    return r;
}

// ----------------------------------------------------------------------

size_t
BufferedSocket::dataAvailable()
{
    return _rbuffer->readAvailable();
}

// ----------------------------------------------------------------------

size_t
BufferedSocket::flushAvailable()
{
    if ( _wbuffer )
        return _wbuffer->readAvailable();
    return 0;
}

// ----------------------------------------------------------------------

size_t
BufferedSocket::reverse ( size_t offset )
{
    return _rbuffer->reverse(offset);
}

// ----------------------------------------------------------------------

size_t
BufferedSocket::skip ( size_t offset )
{
    return _rbuffer->skip(offset);
}

// ----------------------------------------------------------------------

void
BufferedSocket::rxBufferSize ( size_t sz )
{
    if ( _rbuffer )
        delete _rbuffer;
    _rbuffer = new CircularBuffer(sz);
}


size_t
BufferedSocket::rxBufferSize()
{
    return _rbuffer->size();
}

// ----------------------------------------------------------------------

void
BufferedSocket::txBufferSize ( size_t sz )
{
    if ( _wbuffer )
        delete _wbuffer;
    _wbuffer = new CircularBuffer(sz);
}


size_t
BufferedSocket::txBufferSize()
{
    if ( _wbuffer )
        return _wbuffer->size();
    return 0;
}


void
BufferedSocket::enableTxBuffer()
{
    if ( _wbuffer == NULL )
        _wbuffer = new CircularBuffer();
    _wbx  = true;
}

void
BufferedSocket::disableTxBuffer()
{
    if ( _wbuffer )
        delete _wbuffer;
    _wbx = false;
}

// ----------------------------------------------------------------------

ssize_t
BufferedSocket::bufferedWrite ( const void *vptr, size_t n )
{
    const char*  ptr;
    ssize_t      wt, wtn;
    size_t       nleft;
    int          r;

    if ( _wbx && _wbuffer == NULL )
        return -1;

    wt = 0;
    
    // handle buffers first
    if ( _wbx && _wbuffer->readAvailable() > 0 ) {
        r = this->flush();
        if ( r <= 0 ) {
            if ( _wbuffer->writeAvailable() >= n ) {
                wt = _wbuffer->write(vptr, n);
            }
            return r;
        }
    }

    ptr   = (char*) vptr;
    nleft = n;

    do {
        if ( (wtn = this->nwriten(ptr, nleft)) <= 0 )
            break;

        nleft -= wtn;
        ptr   += wtn;
        wt    += wtn;
    } while ( nleft > 0 );


    if ( nleft > 0 && _wbx && _wbuffer->writeAvailable() >= nleft )
        wt += _wbuffer->write(ptr, nleft);

    if ( wtn < 0 )
        wt = wtn;

    return wt;
}

// ----------------------------------------------------------------------

ssize_t
BufferedSocket::bufferData()
{
    char    *wptr;
    size_t   size;

    ssize_t  rd  = 0;
    ssize_t  rcv = 0;

    do {
        size = _rbuffer->writePtrAvailable();
        wptr = _rbuffer->getWritePtr(&size);

        if ( wptr == NULL ) {
            rd = 0;
            break;
        }

        rcv = this->nreadn(wptr, size);

        if ( rcv < 0 ) {
            rd  = rcv;
            rcv = 0;
        } else {
            rd += rcv;
        }

        _rbuffer->setWritePtr(rcv);

    } while ( rcv > 0 );

    return rd;
}

// ----------------------------------------------------------------------

} // namespace


//  _TCANETPP_BUFFEREDSOCKET_CPP_
