/*
 * TnmsSocket.cpp
 *
 *  Copyright(c) 2008, 2009  Timothy Charlton Arland
 *      Author: tca@charltontechnology.net
 */
#define _SOURCE_TNMSSOCKET_CPP_

#include "TnmsSocket.h"

#include "CidrUtils.h"
#include "StringUtils.h"
using namespace tcanetpp;



namespace tnmsCore {


TnmsSocket::TnmsSocket ( MessageHandler * msgHandler )
    : _authFunctor(new AuthAllFunctor(_authorizations)),
      _msgHandler(msgHandler),
      _compression(TNMS_COMPRESSION_ENABLE),
      _sock(NULL),
      _hdr(NULL),
      _rxbuff(NULL),
      _zxbuff(NULL),
      _zipper(NULL),
      _zipout(NULL),
      _wxcbuff(NULL),
      _wptr(NULL),
      _loginCtr(0),
      _clTimeout(DEFAULT_TNMS_CLIENT_TIMEOUT),
      _reconTime(DEFAULT_TNMS_RECONNECT_INTERVAL),
      _lastWxTime(0),
      _wxTimeout(DEFAULT_TNMS_CLIENT_TIMEOUT),
      _flush(TNMS_FLUSH_ENABLE),
      _flushLimit(DEFAULT_TNMS_FLUSH_LIMIT),
      _maxMessages(TNMS_RECORD_LIMIT)
{
    this->_msgHandler = msgHandler;
    this->init();
}


TnmsSocket::TnmsSocket ( const std::string & host, uint16_t port,
                         MessageHandler * msgHandler )
    : _authFunctor(new AuthAllFunctor(_authorizations)),
      _msgHandler(msgHandler),
      _compression(TNMS_COMPRESSION_ENABLE),
      _hostname(host),
      _port(port),
      _sock(NULL),
      _hdr(NULL),
      _rxbuff(NULL),
      _zxbuff(NULL),
      _zipper(NULL),
      _zipout(NULL),
      _wxcbuff(NULL),
      _wptr(NULL),
      _loginCtr(0),
      _clTimeout(DEFAULT_TNMS_CLIENT_TIMEOUT),
      _reconTime(DEFAULT_TNMS_RECONNECT_INTERVAL),
      _lastWxTime(0),
      _wxTimeout(DEFAULT_TNMS_CLIENT_TIMEOUT),
      _flush(TNMS_FLUSH_ENABLE),
      _flushLimit(DEFAULT_TNMS_FLUSH_LIMIT),
      _maxMessages(TNMS_RECORD_LIMIT)
{
    this->init();
}


TnmsSocket::TnmsSocket ( BufferedSocket * sock, MessageHandler * msgHandler )
    : _authFunctor(new AuthAllFunctor(_authorizations)),
      _msgHandler(msgHandler),
      _compression(TNMS_COMPRESSION_ENABLE),
      _sock(sock),
      _hdr(NULL),
      _rxbuff(NULL),
      _zxbuff(NULL),
      _zipper(NULL),
      _zipout(NULL),
      _wxcbuff(NULL),
      _wptr(NULL),
      _loginCtr(0),
      _clTimeout(DEFAULT_TNMS_CLIENT_TIMEOUT),
      _reconTime(DEFAULT_TNMS_RECONNECT_INTERVAL),
      _lastWxTime(0),
      _wxTimeout(DEFAULT_TNMS_CLIENT_TIMEOUT),
      _flush(TNMS_FLUSH_ENABLE),
      _flushLimit(DEFAULT_TNMS_FLUSH_LIMIT),
      _maxMessages(TNMS_RECORD_LIMIT)
{
    if ( sock == NULL )
        throw Exception("TnmsSocket(): invalid socket");

    this->init();
}


TnmsSocket::~TnmsSocket()
{
    this->close();

    if ( _authFunctor )
        delete _authFunctor;
    if ( _msgHandler )
        delete _msgHandler;
    if ( _sock )
        delete _sock;

    if ( _rxbuff )
        ::free(_rxbuff);
    if ( _zxbuff )
        ::free(_zxbuff);

    if ( _wxcbuff )
        delete _wxcbuff;
}


// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //

void
TnmsSocket::init()
{
    // initialize state
    _connecting     = false;
    _authorizing    = false;
    _authorized     = false;
    _subscribed     = false;
    _subtree        = false;
    _loginCtr       = 0;

    // buffers
    _wxcbuff        = new CircularBuffer(TNMS_PACKET_SIZE * 2);
    _rxbuff         = (char*) malloc((size_t) TNMS_PACKET_SIZE);
    _zxbuff         = (char*) malloc((size_t) TNMS_PACKET_SIZE);
    _rxbuffsz       = (size_t) TNMS_PACKET_SIZE;
    _zxbuffsz       = (size_t) TNMS_PACKET_SIZE;
    _lastWxTime     = 0;
    _wxstallsz      = 0;
    _rxCtr          = 0;
    _txCtr          = 0;

    this->clearState();

    if ( this->_sock ) {
        _sock->setNonBlocking();
        _sock->rxBufferSize(TNMS_PACKET_SIZE);
        this->setHostStr();
    }

    if ( _msgHandler == NULL )
        _msgHandler = new MessageHandler();

    return;
}

// ------------------------------------------------------------------- //


int
TnmsSocket::openConnection ( const std::string & host, uint16_t port )
{
    ipv4addr_t addr = CidrUtils::GetHostAddr(host);

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

    _sock = new BufferedSocket(addr, _port, SOCKET_CLIENT, SOCKET_TCP);
    _sock->rxBufferSize(TNMS_PACKET_SIZE);
    _sock->setNonBlocking();

    this->setHostStr();

    return this->connect();
}


int
TnmsSocket::openConnection()
{
    if ( _sock == NULL ) {
        _errstr = "No socket";
        return -1;
    }

    if ( ! _connecting && _sock->isConnected() ) {
        _errstr = "Socket already connected";
        return -1;
    } else if ( ! _connecting ) {
        _sock->init(false);
    }

    int conn = 0;

    if ( (conn = _sock->connect()) > 0 ) {
        _connecting = false;
        _sock->setNonBlocking();
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
        return;

    if ( _wxcbuff )
        this->clearState();

    this->_connecting    = false;
    this->_authorizing   = false;
    this->_authorized    = false;
    this->_subscribed    = false;
    this->_loginCtr      = 0;
    this->_rxCtr         = 0;
    this->_txCtr         = 0;

    //this->clearStall()
    _sock->close();

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
        return this->openConnection();
    } else if ( ! this->isConnected() ) {
        _errstr = "TnmsSocket::send() no connection: ";
        _errstr.append(_hoststr);
        return -1;
    }

    wt = this->flush();

    if ( wt < 0 ) {
        if ( _errstr.empty() )
            _errstr = "TnmsSocket::send() error in flush()";
        return -1;
    }

    fl = _sock->flushAvailable() + _wxcbuff->readAvailable();

    if ( wt == 0 && fl > 0 ) 
    {
        if ( _lastWxTime > 0 ) 
        {
            if ( _wxstallsz <= (size_t) fl ) 
            {
                if ( (_lastWxTime + _wxTimeout) < now ) {
                    _errstr = "TnmsSocket::send() client stalled, timeout exceeded";
                    return -1;
                }
                _wxstallsz = fl;
            } 
            else 
            {
                _lastWxTime = 0;
                _wxstallsz  = 0;
            }
        } 
        else 
        {
            _lastWxTime = now;
            _wxstallsz  = fl;
        }
    } 
    else if ( wt > 0 ) 
    {
        _lastWxTime = 0;
        _wxstallsz  = 0;
        _txCtr     += wt;
    }

    return wt;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::receive ( const time_t & now )
{
    tnmsHeader  hdr;
    size_t      hdrlen;
    ssize_t     rd;
    int         mctr, actr, ctr;

    hdrlen  = sizeof(hdr);
    rd      = 0;
    mctr    = 0;
    actr    = 0;
    ctr     = 0;

    if ( _sock == NULL )
        return -1;

    if ( this->isConnecting() )
        return this->openConnection();
    else if ( ! this->isConnected() ) {  // connection lost
        _errstr = "TnmsSocket::receive() connection lost";
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

        if ( hdr.payload_size > TNMS_PACKET_SIZE ) {
            _errstr = "Payload too large";
            return -1;
        } else if ( hdr.payload_size > _sock->dataAvailable() ) {
            _sock->reverse(hdrlen);
            return ctr;
        }

        if ( hdr.options & ZLIB_COMPRESSED ) {
            rd = this->uncompress(hdr.payload_size);
            if ( rd > 0 )
                hdr.payload_size = rd;
        } else {
            rd = _sock->read((void*)_rxbuff, hdr.payload_size);
        }

        if ( (size_t) rd != hdr.payload_size ) {
            if ( rd < 0 ) {
                _errstr = "TnmsSocket::receive() payload size mismatch: ";
                _errstr.append(_hoststr);
                return -1;
            }
            _sock->reverse(rd + hdrlen);
            return ctr;
        }

        this->_rxCtr += rd;  // track rcv bytes

        if ( _msgHandler )
            ctr = this->receiveMessages(hdr);

    } while ( rd > 0 );

    return ctr;
}

// ------------------------------------------------------------------- //
/**  Note that the TnmsSocket object claims no ownership of a given
 *   message handler. Since the message handlers could potentially be 
 *   used as a singleton, deleting is left to implementation using this
 *   class
 **/
void
TnmsSocket::setMessageHandler ( MessageHandler * msgHandler )
{
    if ( this->_msgHandler )
        delete _msgHandler;
    this->_msgHandler = msgHandler;
    if ( this->_msgHandler == NULL )
        this->_msgHandler = new MessageHandler();
}

const MessageHandler*
TnmsSocket::getMessageHandler()
{
    return this->_msgHandler;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::isConnected()
{
    if ( _sock == NULL )
        return false;

    if ( this->isConnecting() ) {
        if ( this->openConnection() >=0 )
            return true;
        else
            this->close();
    }

    return _sock->isConnected();
}

bool
TnmsSocket::isConnecting()
{
    return this->_connecting;
}

// ------------------------------------------------------------------- //

sockfd_t
TnmsSocket::getDescriptor() const
{
    if ( _sock == NULL )
        return -1;

    return this->_sock->getFD();
}

// ------------------------------------------------------------------- //

void
TnmsSocket::setReconnectTime ( const time_t & secs )
{
    this->_reconTime = secs;
}

const time_t&
TnmsSocket::getReconnectTime() const
{
    return this->_reconTime;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::setCompression ( bool compress )
{
    this->_compression = compress;

    if ( compress ) {
        if ( _zxbuff )
            ::free(_zxbuff);
        _zxbuff  = (char*) malloc((size_t) TNMS_PACKET_SIZE);
    } else {
        if ( _zxbuff )
            ::free(_zxbuff);
        _zxbuff = NULL;
    }
    return;
}


bool
TnmsSocket::getCompression() const
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
TnmsSocket::getTxQueueSize() const
{
    return this->_wxcbuffsz;
}

// ------------------------------------------------------------------- //

size_t
TnmsSocket::getBytesSent()
{
    size_t sz = _txCtr;
    _txCtr = 0;
    return sz;
}

size_t
TnmsSocket::getBytesReceived()
{
    size_t sz = _rxCtr;
    _rxCtr = 0;
    return sz;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::login ( const std::string & user, const std::string & pw )
{
    if ( _login.compare(user) != 0 )
        _login = user;
    if ( _authkey.compare(pw) != 0 )
        _authkey = pw;

    if ( ! this->isConnected() ) {
        _authorizing = false;
        _authorized  = false;
        _loginCtr    = 0;
        return false;
    }

    if ( _authorizing && _loginCtr > TNMS_LOGIN_ATTEMPTS ) {
        this->close();
        return false;
    } else {
        _loginCtr++;
        _authorizing = true;
    }

    TnmsAuthRequest  req(_login, _authkey);

    bool r = this->sendMessage(&req, true);

    return r;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::isAuthorized() const
{
    return this->_authorized;
}

bool
TnmsSocket::isAuthorizing() const
{
    return this->_authorizing;
}

bool
TnmsSocket::isSubscribed() const
{
    return this->_subscribed;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::setClientLogin ( const std::string & login, const std::string & pw )
{
    this->_login   = login;
    this->_authkey = pw;
}

const std::string&
TnmsSocket::getClientLoginName() const
{
    return this->_login;
}

const std::string&
TnmsSocket::getClientAuthStr() const
{
    return this->_authname;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::setHostStr()
{
    if ( _sock == NULL )
        return;

    if ( _hostname.length() == 0 )
        _hostname = _sock->getAddrString();
    if ( _port == 0 )
        _port = _sock->getPort();

    _addrstr = _sock->getAddrString();

    std::ostringstream strbuf;
    strbuf << _hostname << "(" << _addrstr << "):" << _port;
    _hoststr = strbuf.str();

    return;
}

// ------------------------------------------------------------------- //

const std::string&
TnmsSocket::getHostStr() const
{
    return this->_hoststr;
}

// ------------------------------------------------------------------- //

ipv4addr_t
TnmsSocket::getAddr() const
{
    ipv4addr_t  ip = 0;

    if ( this->_sock )
        ip = this->_sock->getAddr();

    return ip;
}

// ------------------------------------------------------------------- //

const std::string&
TnmsSocket::getAddrStr() const
{
    return this->_addrstr;
}

// ------------------------------------------------------------------- //

const std::string&
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

const std::string&
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
TnmsSocket::setFlushLimit ( uint32_t limit )
{
    if ( limit > this->_maxMessages )
        limit = this->_maxMessages;
    this->_flushLimit = limit;
}

// ------------------------------------------------------------------- //

uint32_t
TnmsSocket::getFlushLimit()
{
    return this->_flushLimit;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::setTimeout ( const time_t & secs )
{
    this->_clTimeout = secs;
}

// ------------------------------------------------------------------- //

const time_t&
TnmsSocket::getTimeout() const
{
    return this->_clTimeout;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::subscribeCount()
{
    return this->_subs.size();
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::resubscribe()
{
    if ( ! this->isAuthorized() ) {
        _subscribed = false;
        return _subscribed;
    }

    SubscriptionList::iterator  sIter;
    for ( sIter = _subs.begin(); sIter != _subs.end(); ++sIter ) {
        if ( (_subscribed = this->sendMessage(&(*sIter))) == false )
            break;
    }

    return _subscribed;
}
// ------------------------------------------------------------------- //

bool
TnmsSocket::subscribeStructure()
{
    this->_subtree = true;

    if ( ! this->initHeader(SUBSCRIBE_STRUCTURE, 0) )
        return false;

    if ( _hdr )
        _hdr->options |= LAST_MESSAGE;

    if ( this->flush() < 0 )
        return false;

    return true;
}

bool
TnmsSocket::unsubscribeStructure()
{
    this->_subtree = false;

    if ( ! this->initHeader(UNSUBSCRIBE_STRUCTURE, 0) )
        return false;

    if ( _hdr )
        _hdr->options |= LAST_MESSAGE;

    if ( this->flush() < 0 )
        return false;

    return true;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::subscribeAll()
{
    return this->subscribe("*");
}

bool
TnmsSocket::unsubscribeAll()
{
    return this->unsubscribe("*");
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::subscribe ( const std::string & name )
{
    TnmsSubscribe msg(name);
    _subs.insert(msg);
    _subscribed = this->sendMessage(&msg, true);
    return _subscribed;
}

bool
TnmsSocket::subscribe ( const TnmsOid & oid )
{
    TnmsSubscribe msg(oid);
    bool r = this->sendMessage(&msg, true);
    return r;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::unsubscribe ( const std::string & name )
{
    TnmsUnsubscribe msg(name);
    _subs.erase(msg);
    if ( _subs.size() == 0 )
        _subscribed = false;
    bool r = this->sendMessage(&msg, true);
    return r;
}

bool
TnmsSocket::unsubscribe ( const TnmsOid & oid )
{
    TnmsUnsubscribe  msg(oid);
    bool r = this->sendMessage(&msg, true);
    return r;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::subscribeLevel ( const std::string & name )
{
    char         pf, delim;
    std::string  sub = name;

    if ( name.empty() )
        return false;

    delim = TNMS_DELIMITER_CHAR;
    pf    = name[name.length() - 1];

    if ( pf != delim )
        sub.append(TNMS_DELIMITER);

    return this->subscribe(sub);
}


bool
TnmsSocket::unsubscribeLevel ( const std::string & name )
{
    char         pf, delim;
    std::string  unsub;

    if ( name.empty() )
        return false;

    delim = TNMS_DELIMITER_CHAR;
    pf    = name[name.length() - 1];

    if ( pf != delim )
        unsub.append(TNMS_DELIMITER);

    return this->unsubscribe(unsub);
}

// ------------------------------------------------------------------- //

SubscriptionList&
TnmsSocket::getSubscriptionList()
{
    return this->_subs;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::authReply ( const TnmsAuthReply & reply )
{
    this->_authorizing = false;

    if ( reply.authResult() == AUTH_SUCCESS )
        this->_authorized = true;
    else
        this->_authorized = false;

    if ( ! this->_authorized )
        return;

    std::string::size_type  pos;

    _authorizations.clear();
    if ( _authFunctor )
        delete _authFunctor;

    pos = reply.authReason().find_first_of(":");

    if ( pos != std::string::npos ) {
        StringUtils::split(reply.authReason(), ':', std::back_inserter(_authorizations));
        std::string isInc  = _authorizations.back();

        if ( isInc.compare("<I>") == 0 )
            _authFunctor = new AuthIncludeFunctor(_authorizations);
        else
            _authFunctor = new AuthExcludeFunctor(_authorizations);
    } else {
        _authFunctor = new AuthAllFunctor(_authorizations);
    }

    return;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendMessage ( const Serializable * message, bool flushNow )
{
    size_t  msz, wt;

    msz = message->size();
    wt  = 0;

    if ( ! this->initHeader(message->message_type(), msz) )
        return false;

    wt = message->serialize(_wptr, (_wtsize - _wtt));

    if ( wt < 0 ) {
        _errstr = "TnmsSocket::sendMessage() error in message serialization for " + this->getHostStr();
        return false;
    }

    _wptr += wt;
    _wtt  += wt;

    _hdr->message_count++;
    _hdr->payload_size += wt;

    if ( flushNow )
        if ( this->flush() < 0 )
            return false;

    return true;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::receiveMessages ( tnmsHeader & hdr )
{
    int  ctr = 0;

    switch ( hdr.message_type ) {
        case AUTH_REQUEST:
            ctr += this->rcvAuthRequest(hdr);
            break;
        case AUTH_REPLY:
            ctr += this->rcvAuthReply(hdr);
            break;
        case AUTH_REFRESH:
            break;
        case ADD_MESSAGE:
            ctr += this->rcvAdds(hdr);
            break;
        case REMOVE_MESSAGE:
            ctr += this->rcvRemoves(hdr);
            break;
        case REQUEST_MESSAGE:
            ctr += this->rcvRequests(hdr);
            break;
        case METRIC_MESSAGE:
            ctr += this->rcvMetrics(hdr);
            break;
        case SUBSCRIBE:
            ctr += this->rcvSubscribes(hdr);
            break;
        case UNSUBSCRIBE:
            ctr += this->rcvUnsubscribes(hdr);
            break;
        case SUBSCRIBE_STRUCTURE:
            _msgHandler->StructureHandler(true);
            ctr++;
            break;
        case UNSUBSCRIBE_STRUCTURE:
            _msgHandler->StructureHandler(false);
            ctr++;
            break;
        case PING_REQUEST:
            _msgHandler->PingHandler();
            ctr++;
            break;
        case PING_REPLY:
            _msgHandler->PingReplyHandler();
            ctr++;
            break;
        default:
            //unknown type
            break;
    }

    if ( hdr.options & LAST_MESSAGE )
        _msgHandler->LastMessageHandler(hdr.message_type);

    return ctr;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvAuthRequest ( tnmsHeader & hdr )
{
    char    * rptr;
    size_t    rsz, rd;
    ssize_t   upk;

    TnmsAuthRequest  auth;

    rptr  = _rxbuff;
    rsz   = hdr.payload_size;
    rd    = 0;
    upk   = 0;

    upk = auth.deserialize(rptr, rsz);

    if ( upk < 0 ) {
        _errstr = "TnmsSocket::rcvAuthRequest() Error in deserialize";
        return -1;
    }

    _msgHandler->AuthRequestHandler(auth);

    return 1;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvAuthReply ( tnmsHeader & hdr )
{
    char   * rptr;
    size_t   rsz, rd;
    ssize_t  upk;

    TnmsAuthReply  reply;

    rptr  = _rxbuff;
    rsz   = hdr.payload_size;
    rd    = 0;

    upk = reply.deserialize(rptr, rsz);

    if ( upk < 0 ) {
        _errstr = "TnmsSocket::rcvAuthReply() error in deserialize";
        return -1;
    }

    _msgHandler->AuthReplyHandler(reply);

    return 1;
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

    for ( i = 0; i < hdr.message_count; ++i ) {
        TnmsMetric  metric;

        upk = metric.deserialize(rptr, rsz-rd);

        if ( upk < 0 ) {
            _errstr = "TnmsSocket::rcvMetrics() Error in deserialize";
            break;
        }

        rd   += upk;
        rptr += upk;

        _msgHandler->MetricHandler(metric);
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

    rptr = _rxbuff;
    rsz  = hdr.payload_size;
    rd   = 0;
    upk  = 0;

    for ( i = 0; i < hdr.message_count; ++i ) {
        TnmsAdd  addmsg;

        upk = addmsg.deserialize(rptr, (rsz-rd));

        if ( upk < 0 ) {
            _errstr = "TnmsSocket::rcvAdds() Error in deserialize";
            break;
        }

        rd   += upk;
        rptr += upk;

        _msgHandler->AddHandler(addmsg);
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

    rptr = _rxbuff;
    rsz  = hdr.payload_size;
    rd   = 0;
    upk  = 0;

    for ( i = 0; i < hdr.message_count; ++i ) {
        TnmsRemove  remove;

        upk  = remove.deserialize(rptr, (rsz-rd));

        if ( upk < 0 ) {
            _errstr = "TnmsSocket::rcvRemoves() Error in deserialize";
            break;
        }

        rd   += upk;
        rptr += upk;

        _msgHandler->RemoveHandler(remove);
    }

    return i;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvSubscribes ( tnmsHeader & hdr )
{
    char   * rptr;
    size_t   rsz, rd;
    ssize_t  upk, i;

    rptr = _rxbuff;
    rsz  = hdr.payload_size;
    rd   = 0;
    upk  = 0;

    for ( i = 0; i < hdr.message_count; ++i ) {
        TnmsSubscribe  sub;

        upk = sub.deserialize(rptr, (rsz-rd));

        if ( upk < 0 ) {
            _errstr = "TnmsSocket::rcvSubscribes() Error in deserialize.";
            break;
        }

        rd   += upk;
        rptr += upk;

        _msgHandler->SubscribeHandler(sub.getElementName());
    }

    return i;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvUnsubscribes ( tnmsHeader & hdr )
{    
    char   * rptr;
    size_t   rsz, rd;
    ssize_t  upk, i;

    rptr = _rxbuff;
    rsz  = hdr.payload_size;
    rd   = 0;
    upk  = 0;

    for ( i = 0; i < hdr.message_count; ++i ) {
        TnmsSubscribe  unsub;

        upk = unsub.deserialize(rptr, (rsz-rd));

        if ( upk < 0 ) {
            _errstr = "TnmsSocket::rcvUnsubscribes() Error in deserialize.";
            break;
        }

        rd   += upk;
        rptr += upk;

        _msgHandler->UnsubscribeHandler(unsub.getElementName());
    }

    return i;
}

// ------------------------------------------------------------------- //

int
TnmsSocket::rcvRequests ( tnmsHeader & hdr )
{
    char   * rptr;
    size_t   rsz, rd;
    ssize_t  upk, i;

    rptr = _rxbuff;
    rsz  = hdr.payload_size;
    rd   = 0;
    upk  = 0;

    for ( i = 0; i < hdr.message_count; ++i ) {
        TnmsRequest  req;

        upk = req.deserialize(rptr, (rsz-rd));

        if ( upk < 0 ) {
            _errstr = "TnmsSocket::rcvRequests() Error in deserialize.";
            break;
        }

        rd   += upk;
        rptr += upk;

        _msgHandler->RequestHandler(req);
    }

    return i;
}

// ------------------------------------------------------------------- //

void
TnmsSocket::clearState()
{
    _hdr     = NULL;
    _wptr    = NULL;
    _wtt     = 0;
    _wtsize  = 0;

    if ( this->getCompression() ) {
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
TnmsSocket::setLastRecord()
{
    if ( _hdr )
        _hdr->options |= LAST_MESSAGE;
}

// ------------------------------------------------------------------- //

size_t
TnmsSocket::txBytesBuffered()
{
    if ( _sock )
        return _sock->flushAvailable() + _wxcbuff->readAvailable();
    return 0;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::initHeader ( uint16_t type, size_t messagesize )
{
    size_t  sz, hdrlen, rd;

    hdrlen = sizeof(tnmsHdr_t);
    sz     = messagesize;
    rd     = 0;

    if ( _sock == NULL || ! this->isConnected() )
        return false;

    if ( _wptr )  // update output buffer
    {
        if ( _compression && (type == METRIC_MESSAGE
                          ||  type == ADD_MESSAGE || type == REMOVE_MESSAGE) )
        {
            // write to zxbuff
        }
        else 
        {
            _wxcbuff->setWritePtr(_wtt);
            _wptr   = NULL;
        }
    }

    if ( _hdr )    // check current header
    {
        
        if ( _flush && _hdr->message_count >= _flushLimit )   // check flush limit
        {
            if ( _hdr->message_type != type )
                _hdr->options |= LAST_MESSAGE;
            rd = this->flush();

        }
        else if ( _hdr->message_type == type )   // continue pdu
        {
            if ( _compression && (type == METRIC_MESSAGE
                              ||  type == ADD_MESSAGE || type == REMOVE_MESSAGE) )
            {
                if ( _wtsize < (_wtt + sz) ) {  // buffer full, stop queue'ing
                    this->flush();
                    return false;
                }
            }
            else
            {
                _wptr   = _wxcbuff->getWritePtr(&sz);
                _wtsize = sz;  // store space avail for writing
                _wtt    = 0;   // init write total for this write session

                if ( _wptr == NULL ) 
                {
                    _errstr = "TnmsSocket::initHeader() out of space: ";
                    _errstr.append(_hoststr);
                    _wxcbuff->setWritePtr(0);
                    this->flush();
                    return false;
                }
            }

            return true;

        }
        else   // flush pdu 
        {
            _hdr->options |= LAST_MESSAGE;
            rd = this->flush();
        }
    }

    sz    = messagesize + hdrlen;
    _wptr = _wxcbuff->getWritePtr(&sz);

    if ( _wptr == NULL || sz < (messagesize+hdrlen) )
    {
        _wxcbuff->setWritePtr(0);
        this->clearState();
        _errstr = "ClientSocket::initHeader() out of space: ";
        _errstr.append(_hoststr);
        return false;
    }

    // initialize new header
    _hdr     = (tnmsHdr_t*) _wptr;
    _wptr   += hdrlen;
    _wtt     = hdrlen;
    _wtsize  = (sz - _wtt);

    _hdr->major_version  = TNMS_VERSION_MAJOR;
    _hdr->minor_version  = TNMS_VERSION_MINOR;
    _hdr->options        = 0;
    _hdr->payload_size   = 0;
    _hdr->message_count  = 0;
    _hdr->message_type   = type;

    // reset compression buffer if applicable
    if ( _compression && (type == METRIC_MESSAGE || type == ADD_MESSAGE
                      ||  type == REMOVE_MESSAGE) )
    {
        _hdr->options |= ZLIB_COMPRESSED;
        _zipout  = new std::ostringstream(std::ios::out);
        _zipper  = new zip_ostream(*_zipout, true);
    }

    return true;
}

// ------------------------------------------------------------------- //

ssize_t
TnmsSocket::flush()
{
    char    *rptr;
    size_t   sz, hdrlen;
    ssize_t  wt, tt;

    hdrlen = sizeof(tnmsHdr_t);
    wt     = 0;
    tt     = 0;

    if ( _sock == NULL || ! this->isConnected() )
        return -1;

    if ( _sock->flushAvailable() > 0 )
        wt = _sock->flush();

    if ( wt < 0 )
        return wt;
    else if ( wt > 1 )
        tt += wt;

    if ( _hdr && _hdr->message_count > 0 ) {
        wt = 0;

        if ( _compression && (_hdr->message_type == METRIC_MESSAGE
                          ||  _hdr->message_type == ADD_MESSAGE
                          ||  _hdr->message_type == REMOVE_MESSAGE) )
        {
            size_t len;

            _zipper->zflush();
            len  = _zipout->str().length();
            _hdr->payload_size = len;

            if ( _wxcbuff->writeAvailable() < len ) {
                _errstr = "TnmsSocket::flush() write buffer out of space: ";
                _errstr.append(_hoststr);
            } else {
                _wxcbuff->setWritePtr(hdrlen);
                wt = _wxcbuff->write(_zipout->str().c_str(), len);
                if ( (size_t) wt < len ) {
                    _errstr = "TnmsSocket::flush() error flushing compressed data: ";
                    _errstr.append(_hoststr);
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

    sz  = _sock->read((void*)_zxbuff, size);

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


