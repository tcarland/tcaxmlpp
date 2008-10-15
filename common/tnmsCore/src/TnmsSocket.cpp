#define _SOURCE_TNMSSOCKET_CPP_

#include "TnmsSocket.h"



namespace tnmscore {


TnmsSocket::TnmsSocket ( uint32_t  flush_limit )
    : _authFunctor(new AuthAllFunctor()),
      _connecting(false),
      _authenticating(false),
      _authorized(false),
      _subscribed(false),
      _subtree(false),
      _compression(DEFAULT_COMPRESSION_ENABLE),
      _sock(NULL),
      _hdr(NULL),
      _rxbuff(NULL),
      _zxbuff(NULL),
      _zipper(NULL),
      _wxcbuff(NULL),
      _wptr(NULL),
      _clTimeout(TNMS_CLIENT_TIMEOUT),
      _reconTime(TNMS_CLIENT_TIMEOUT),
      _lastWxTime(0),
      _wxTimeout(TNMS_CLIENT_TIMEOUT),
      _loginAttempts(0),
      _flush(DEFAULT_FLUSH_ENABLE),
      _flushLimit(flush_limit),
      _maxMessages(TNMS_RECORD_LIMIT)
{
    this->init();
}


TnmsSocket::TnmsSocket ( ipv4addr_t ip, uint16_t port,
                         uint32_t  flush_limit )
    : _authFunctor(new AuthAllFunctor()),
      _connecting(false),
      _authenticating(false),
      _authorized(false),
      _subscribed(false),
      _subtree(false),
      _compression(DEFAULT_COMPRESSION_ENABLE),
      _sock(NULL),
      _hdr(NULL),
      _rxbuff(NULL),
      _zxbuff(NULL),
      _zipper(NULL),
      _wxcbuff(NULL),
      _wptr(NULL),
      _clTimeout(TNMS_CLIENT_TIMEOUT),
      _reconTime(TNMS_CLIENT_TIMEOUT),
      _lastWxTime(0),
      _wxTimeout(TNMS_CLIENT_TIMEOUT),
      _loginAttempts(0),
      _flush(DEFAULT_FLUSH_ENABLE),
      _flushLimit(flush_limit),
      _maxMessages(DEFAULT_MAXMESSAGES)
{
    this->init();
}


TnmsSocket::TnmsSocket ( BufferedSocket * sock, uint32_t flush_limit )
    : _authFunctor(new AuthAllFunctor()),
      _connecting(false),
      _authenticating(false),
      _authorized(false),
      _subscribed(false),
      _subtree(false),
      _compression(DEFAULT_COMPRESSION_ENABLE),
      _sock(NULL),
      _hdr(NULL),
      _rxbuff(NULL),
      _zxbuff(NULL),
      _zipper(NULL),
      _wxcbuff(NULL),
      _wptr(NULL),
      _clTimeout(DEFAULT_CLIENT_TIMEOUT),
      _reconTime(DEFAULT_CLIENT_TIMEOUT),
      _lastWxTime(0),
      _wxTimeout(DEFAULT_CLIENT_TIMEOUT),
      _loginAttempts(0),
      _flush(DEFAULT_FLUSH_ENABLE),
      _flushLimit(flush_limit),
      _maxMessages(DEFAULT_MAXMESSAGES)
{
    if ( sock == NULL )
        throw Exception("TnmsSocket(): invalid socket");

    this->init();
}


TnmsSocket::~TnmsSocket() {}


// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //

void
TnmsSocket::init()
{
    // initialize state
    _connecting     = false;
    _authenticating = false;
    _authorized     = false;
    _subscribed     = false;
    _subtree        = false;
    _compression    = DEFAULT_COMPRESSION_ENABLE;
    _timeout        = DEFAULT_CLIENT_TIMEOUT;      // reconnect
    _loginAttemtps  = 0;

    // buffers
    _wxcbuff         = new CirBuffer(MAX_TNMSPACKET_SIZE * 2);
    _rxbuff         = (char*) malloc((size_t) MAX_TNMSPACKET_SIZE);
    _zxbuff         = (char*) malloc((size_t) MAX_TNMSPACKET_SIZE);
    _rxbuffsz       = (size_t) MAX_TNMSPACKET_SIZE;
    _zxbuffsz       = (size_t) MAX_TNMSPACKET_SIZE;
    _lastWxTime     = 0;
    _wxStallSize    = 0;

    this->clearState();

    if ( this->_sock ) {
        _sock->unblock();
        _sock-rxBufferSize(MAX_TNMSPACKET_SIZE);
        _sock->disableTxBuffer();
        this->setHostStr();
    }

    return;
}

// ------------------------------------------------------------------- //


int
TnmsSocket::openConnection ( const std::string & host, uint16_t port )
{
    ipv4addr_t addr = Cidr::getHostAddr(host);

    if ( addr == 0 ) {
        _errstr = "Invalid host ";
        _errstr.append(host);
        return -1;
    }

    _hostname = host;
    _port     = port;

    if ( this->_sock ) {
        if ( _sock->isConnected() )
            this->close();
        delete _sock;
    }

    _sock = new BufferedSocket(addr, _port, SOCKET_CLIENT, TCP);
    _sock->rxBuffersize(MAX_TNMSPACKET_SIZE);
    _sock->disableTxBuffer();
    _sock->unblock();

    this->setHostStr();

    return this->connect();
}


int
TnmsSocket::openConnection()
{
    if ( _sock == NULL )
        return -1;

    if ( ! _connecting && _sock->isConnected() ) {
        _errstr = "Socket already connected";
        return -1;
    } else if ( ! _connecting ) {
        _sock->init(false);
    }

    int conn = 0;

    if ( (conn = _socket->connect()) > 0 ) {
        _connecting = false;
        _sock->unblock();
    } else if ( conn == 0 ) {
        _connecting = true;
        return conn;
    } else {
        _errstr = "Error connecting to host: ";
        _errstr.append(_sock->errorStr());
        _connecting = false;
        _sock->close();
        return conn;
    }

    if ( this->_subtree )
        this->subscribeStructure();

    return conn;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::closeConnection()
{
    if ( _sock == NULL )
        return -1;

    if ( _wxcbuff )
        this->clearState();

    return;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::send ( const time_t & now )
{
    ssize_t  wt, fl;

    if ( _sock == NULL )
        return -1;

    if ( this->isConnecting() ) {
        return this->OpenConnection();
    } else if ( ! this->isConnected() ) {
        return -1;
    }

    wt = this->flush();

    if ( wt < 0 )
        return -1;

    fl = _sock->flushAvailable() + _wxcbuff->fullDataAvailable();

    if ( wt == 0 && fl > 0 ) {
        if ( _lastWxTime > 0 ) {
            if ( _wxstallsz <= (size_t) fl ) {
                if ( (_lastWxTime + _wxTimeout) < now ) {
                    _errstr.append(" TnmsSocket::send() client stalled, timeout exceeded: ");
                    _errstr.append(_hostStr);
                    return -1;
                }
                _wxStallsz = fl;
            } else {
                _lastWxTime = 0;
                _wxstallsz  = 0;
            }
        } else {
            _lastWxTime  = now;
            _wxstallsz = fl;
        }
    } else if ( wt > 0 ) {
        _lastWxTime = 0;
        _wxstallsz  = 0;
    }

    return wt;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::receive()
{
    tnmsHeader  hdr;
    size_t      hdrlen;
    ssize_t     rd;
    int         mctr, actr, ctr;

    hdrlen  = sizeof(tnmsHeader);
    rd      = 0;
    mctr    = 0;
    actr    = 0;
    ctr     = 0;

    if ( _sock == NULL )
        return -1;

    if ( this->isConnecting() )
        return this->openConnection();
    else if ( ! this->isConnected() ) {  // connection lost
        return -1;
    }

    do {
        rd  = _sock->read((void*)&hdr, hdrlen);

        if ( ((size_t) rd) != hdrlen ) {
            if ( rd < 0 )
                return -1;
            _sock->reverse(rd);
            return ctr;
        }

        if ( hdr.payload_size > TNMS_MAXPACKET_SIZE ) {
            _errstr = "Payload too large";
            return -1;
        } else if ( hdr.payload_size > _sock->dataAvailable() ) {
            _sock->reverse(hdrlen);
            return ctr;
        }

        if ( hdr.options & ZLIB_COMPRESSED ) {
            rd = this->uncompressPayload(hdr.payloadsize);
            if ( rd > 0 )
                hdr.payload_size = rd;
        } else {
            rd = _sock->read((void*)_rxbuff, hdr.payload_size);
        }

        if ( (size_t) rd != hdr.payload_size ) {
            if ( rd < 0 ) {
                _errstr = "TnmsSocket::receive() payload size mismatch: ";
                _errstr.append(_hostStr);
                return -1;
            }
            _sock->reverse(rd + hdrlen);
            return ctr;
        }

        switch ( hdr.record_type ) {
            case AUTH_TYPE:
                break;
            case RECORD_METRIC:
                ctr += this->rcvMetrics(hdr);
                break;
            default:
                //unknown type
                break;
        }

        if ( hdr.options & LAST_RECORD )
            this->lastRecordHandler(hdr.record_type);
    } while ( rd > 0 );

    return ctr;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::isConnected() const
{
    if ( _sock == NULL )
        return false;

    if ( _connecting ) {
        if ( this->openConnection() >=0 )
            return true;
        else
            this->close();
    }

    return _sock->isConnected();
}

bool
TnmsSocket::isConnecting() const
{
    return this->_connecting;
}

// ------------------------------------------------------------------- //

const sock_t&
TnmsSocket::GetDescriptor() const
{
    if ( _sock == NULL )
        return -1;

    return this->_sock->getFD();
}

// ------------------------------------------------------------------- //

void
TnmsSocket::reconnectTime ( const time_t & secs )
{
    this->_reconTime = secs;
}


TnmsSocket::reconnectTime()
{
    return this->_reconTime;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::compression ( bool compress )
{
    this->_compression = compress;
}


bool
TnmsSocket::compression() const
{
    return this->_compression;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::setTxQueueSize ( size_t bytes )
{
    this->_wxcbuffsz = bytes;
    if ( _wxcbuff )
        _wxcbuff->resize(bytes);
    return;
}


size_t
TnmsSocket::getTxQueueSize()
{
    return this->_wxcbuffsz;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::login ( const std::string & user, const std::string & pw )
{
    this->_login = user;

}

// ------------------------------------------------------------------- //

bool
TnmsSocket::isAuthorized() const
{
    return this->_authorized;
}

bool
TnmsSocket::isSubscribed() const
{
    return this->_subscribed;
}

// ------------------------------------------------------------------- //

std::string
TnmsSocket::getClientLogin() const
{
    return this->_login;
}

// ------------------------------------------------------------------- //

std::string
TnmsSocket::getHostStr() const
{
    return this->_hoststr;
}

// ------------------------------------------------------------------- //

std::string
TnmsSocket::getAddrStr() const
{
    return this->_addrstr;
}

// ------------------------------------------------------------------- //

std::string
TnmsSocket::getHostname() const
{
    return this->_hostname;
}

// ------------------------------------------------------------------- //

uint16_t
TnmsSocket::getHostPort() const
{
    return this->_port;
}

// ------------------------------------------------------------------- //

uint32_t
TnmsSocket::getVersion() const
{
    return this->_version;
}

// ------------------------------------------------------------------- //

std::string
TnmsSocket::getErrorStr() const
{
    return this->_errstr;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::setDebug ( bool debug )
{
    this->_debug = debug;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::enableFlush ( bool flush )
{
    this->_flush = flush;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::flushEnabled()
{
    return this->_flush;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::setTimeout ( time_t secs )
{
    this->_timeout = secs;
}

// ------------------------------------------------------------------- //

time_t
TnmsSocket::getTimeout()
{
    return this->_timeout;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::subscribeCount()
{
    return this->_subscriptions->size();
}

// ------------------------------------------------------------------- //

void
TnmsSocket::resubscribe()
{}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendMessage ( Serializable * message )
{
    size_t  msz, wt;
    
    msz = message.size();
    wt  = 0;

    if ( ! this->initHeader(message.message_type(), msz) )
        return false;

    wt = message.serialize(_wptr, (_wtsize - _wtt));

    _wptr += wt;
    _wtt  += wt;

    _hdr->record_count++;
    _hdr->payload_size += wt;

    if ( this->flush() < 0 )
        return false;

    return true;
}

// ------------------------------------------------------------------- //
/*
bool
TnmsSocket::sendAuthRequest ( TnmsAuthRequest & request )
{
    size_t  msz, wt;
    
    msz = request.size();
    wt  = 0;

    if ( ! this->initHeader(AUTH_REQUEST, msz) )
        return false;

    wt = request.serialize(_wptr, (_wtsize - _wtt));

    _wptr += wt;
    _wtt  += wt;

    _hdr->record_count++;
    _hdr->payload_size += wt;

    if ( this->flush() < 0 )
        return false;

    return true;
}
// ------------------------------------------------------------------- //

bool
TnmsSocket::sendAuthReply ( TnmsAuthReply & reply )
{
    return false;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendMetric ( TnmsMetricMessage & metric )
{
    return false;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendAdd ( TnmsAddMessage & metric )
{
    return false;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendRemove ( TnmsRemoveMessage & metric )
{
    return false;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendRequest ( TnmsRequestMessage & metric )
{
    return false;
}
*/
// ------------------------------------------------------------------- //

bool
TnmsSocket::setLastRecord()
{
    return false;
}

// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //

int
TnmsSocket::rcvAuthRequest ( tnmsHeader & hdr )
{
    char    * rptr;
    size_t    rsz, rd;
    ssize_t   upk;

    tnmsAuthRequest_t  auth;

    rptr  = _rxbuff;
    rsz   = hdr.payload_size;
    rd    = 0;
    upk   = 0;

    upk = TnmsAuthRequest::Unpack(rptr, rsz, auth);

    if ( upk < 0 ) {
        _errstr = "TnmsSocket::rcvAuthRequest() Error ";
        return -1;
    }

    this->AuthRequestHandler(auth);

    return 1;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvAuthReply ( tnmsHeader & hdr )
{
    return -1;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvMetrics ( tnmsHeader & hdr )
{
    char   * rptr;
    size_t   rsz, rd;
    ssize_t  upk, i;
    
    rptr = _rxbuff;
    rsz  = hdr.payload_size;
    rd   = 0;
    upk  = 0;

    for ( i = 0; i < hdr.record_count; ++i ) {
        TnmsMetric  metric;

        upk = TnmsMetric::unpack(rptr, rsz-rd, metric);

        if ( upk < 0 ) {
            _errstr = "TnmsSocket::rcvMetrics() Error";
            break;
        }

        rd   += upk;
        rptr += upk;

        this->MetricHandler(metric);
    }

    return i;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvAdds ( tnmsHeader & hdr )
{
    char    * rptr;
    size_t    rsz, rd;
    ssize_t   upk, i;

    for ( i = 0; i < hdr.record_count; ++i ) {
        TnmsAdd  addmsg;

        upk = TnmsRequest::Unpack(rptr, rsz - rd, addmsg);

        if ( upk < 0 ) {
            _errstr = "TnmsSocket::rcvAdds() Error unpacking add";
            break;
        }

        rd   += upk;
        rptr += upk;

        this->AddHandler(addmsg);
    }
                
    return i;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvRemoves ( tnmsHeader & hdr )
{
    char   * rptr;
    size_t   rsz, rd;
    ssize_t  upk, i;

    for ( i = 0; i < hdr.record_count; ++i ) {
        TnmsRemove  remove;

        upk  = TnmsRequest::Unpack(rptr, rsz - rd, remove);

        if ( upk < 0 ) {
            _errstr = "TnmsSocket::rcvRemoves() Error unpacking";
            break;
        }

        rd   += upk;
        rptr += upk;

        this->RemoveHandler(remove);
    }

    return i;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvSubscribes ( tnmsHeader & hdr )
{
    return -1;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvUnsubscribes ( tnmsHeader & hdr )
{
    return -1;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvRequests ( tnmsHeader & hdr )
{
    return -1;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::clearState()
{
    _hdr     = NULL;
    _wptr    = NULL;
    _wtt     = 0;
    _wtsize  = 0;

    if ( this->compression() ) {
        if ( _zipper )
            delete _zipper;
        if ( _zipout )
            delete _zipout;
        _zipper = NULL;
        _zipout = NULL;
    }

    return;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::setHostStr()
{
    if ( _sock == NULL )
        return;

    if ( _hostname.length() == 0 )
        _hostname = _sock->getAddrStr();
    if ( _port == 0 )
        _port = _sock->getPort();

    std::ostringstream strbuf;
    strbuf << _hostname << "(" << _sock->getAddrStr() << "):" << _port;
    _hostStr = strbuf.str();
    return;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::initHeader ( int type, size_t messagesize )
{    
    size_t  sz, hdrlen, rd;
    
    hdrlen = sizeof(tnmsHeader_t);
    sz     = messagesie;
    rd     = 0;

    if ( _sock == NULL || ! this->IsConnected() )
        return false;

    if ( _wptr ) { // update output buffer
        if ( _compression && (type == METRIC_TYPE 
                          || type == ADD_TYPE || type == REMOVE_TYPE) )
        {} 
        else {
            _wxcbuff->setWritePtr(_wtt);
            _wptr   = NULL;
        }
    }

    // check current header
    if ( _hdr ) {   // check flush limit

        if ( _flush && _hdr->field_count >= _flushLimit ) {
            if ( _hdr->record_type != type )
                _hdr->options |= LAST_RECORD;
            rd = this->FlushQueue();
        
        } else if ( _hdr->record_type == type ) {  // continue pdu

            if ( _compression && (type == METRIC_TYPE 
                              || type == ADD_TYPE || type == REMOVE_TYPE) )
            {
                if ( _wtsize < (_wtt + recordsize) ) {  // buffer full, stop queue'ing
                    this->FlushQueue();
                    return false;
                }
            } 
            else 
            {
                _wptr   = _wxcbuff->getWritePtr(&sz);
                _wtsize = sz;  // store space avail for writing
                _wtt    = 0;   // init write total for this write session

                if ( _wptr == NULL ) {
                    _errstr = "TnmsSocket::initHeader() out of space: ";
                    _errstr.append(_connstr);
                    _wxcbuff->setWritePtr(0);
                    this->flush();
                    return false;
                }
            }

            return true; 
        
        } else {   // flush pdu
            _hdr->options |= LAST_RECORD;
            rd = this->flush();
        }
    }
    
    sz    = messagesize + hdrlen;
    _wptr = _wxcbuff->getWritePtr(&sz);

    if ( _wptr == NULL || sz < (messagesize+hdrlen) ) {
        _wxcbuff->setWritePtr(0);
        this->clearState();
        _errstr = "ClientSocket::initHeader() out of space: ";
        _errstr.append(_connstr);
        return false;
    }

    // initialize new header
    _hdr     = (tnmsHeader_t*) _wptr;
    _wptr   += hdrlen;
    _wtt     = hdrlen;
    _wtsize  = (sz - _wtt);

    _hdr->version       = TNMSCORE_VERSION_MAJOR | TNMSCORE_VERSION_MINOR;
    _hdr->options       = 0;
    _hdr->payload_size  = 0;
    _hdr->field_count   = 0;
    _hdr->record_type   = type;

    // reset compression buffer if applicable
    if ( _compression && (type == RECORD_METRIC || type == RECORD_ADD 
                      ||  type == RECORD_REMOVE) )
    {
        _hdr->options |= ZLIB_COMPRESSED;
        _zipout  = new std::ostringstream(std::ios::out);
        _zipper  = new zip_ostream(*_zipout, true);
    }

    return true;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::flush()
{
    char    *rptr;
    size_t   sz, hdrlen;
    ssize_t  wt, tt;

    hdrlen = sizeof(tnmsHeader_t);

    if ( _sock == NULL || ! this->isConnected() )
        return -1;

    if ( _sock->flushAvailable() > 0 )
        wt = _sock->flush();

    if ( wt < 0 )
        return wt;
    else if ( wt > 1 ) 
        tt += wt;

    if ( _hdr && _hdr->record_count > 0 ) {
        wt = 0;

        if ( _compression && (_hdr->record_type == RECORD_METRIC
                          ||  _hdr->record_type == RECORD_ADD
                          ||  _hdr->record_type == RECORD_REMOVE) )
        {
            size_t len;

            _zipper->zflush();
            len = _zipout->str().length();
            _hdr->payload_size = len;

            if ( _wxcbuff->fullSpaceAvailable < len ) {
                _errstr = "TnmsSocket::flush() write buffer out of space: ";
                _errstr.append(_hostStr);
            } else {
                _wxcbuff->setWritePtr(hdrsz);
                wt = _wxcbuff->write(_zipout->str().c_str(), len);
                if ( (size_t) wt < len ) {
                    _errstr = "TnmsSocket::flush() error flushing compressed data: ";
                    _errstr.append(_hostStr);
                    return -1;
                }
            }
        }
        else  // no compression
        {
            if ( _wptr )
                _wxcbuff->setWritePtr(_wtt);
            wt = _wtt;
        }

        if ( wt > 0 )
            this->clearState();
    }

    rptr = _wxcbuff->getReadPtr(&sz);
    if ( rptr == NULL )
        return wt;

    wt   = _sock->write(rptr, sz);
    if ( wt < 0 )
        return wt;

    _wxcbuff->setReadPtr(wt);
    tt  += wt;

    return tt;
}

// ------------------------------------------------------------------- //

ssize_t
TnmsSocket::uncompress ( uint32_t size )
{
    char   * rptr = NULL;
    ssize_t  sz;

    sz  = _sock->read((void*)_zxbuff, tsz);

    if ( (size_t) sz != size ) {
        _errstr = "TnmsSocket::uncompress() error uncompressing payload";
        return -1;
    }

    std::ostringstream  outbuff;
    std::streamsize     in;

    outbuff.write(_zxbuff, size);

    std::istringstream  istream(outbuff.str(), std::ios::in);
    zip_istream         unzipper(istream);

    rptr = _rxbuff;
    sz   = 0;

    while ( unzipper.peek() != EOF ) {
        in = unzipper.rdbuf()->in_avail();
        if ( ((size_t)in + sz) > _rxbuffsz ) {
            _rxbuffsz += in;
            _rxbuff    = (char*) ::realloc(_rxbuff, _rxbuffsz);

            if ( _rxbuff == NULL ) {
                _errstr = "ClientSocket::uncompress() Fatal error in realloc of rxbuff";
                return -1;
            }

            rptr = _rxbuff + sz;
        }

        unzipper.read(rptr, in);
        rptr += in;
        sz   += in;
    }

    unzipper.read_footer();
    
    //size_t zin    = unzipper.rdbuf()->get_in_size();
    size_t unzout = unzipper.rdbuf()->get_out_size();

    if ( unzout == 0 )
        return -1;

    return unzout;
}

// ------------------------------------------------------------------- //

} // namespace


