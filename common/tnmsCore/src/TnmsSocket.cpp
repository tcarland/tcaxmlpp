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
      _wxbuff(NULL),
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
      _wxbuff(NULL),
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
      _wxbuff(NULL),
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
    _wxbuff         = new CirBuffer(MAX_TNMSPACKET_SIZE * 2);
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

    if ( _wxbuff )
        this->clearState();

    return;
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
    return this->_wxbuffsz;
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
TnmsSocket::subscribes()
{
    return this->_subscriptions->size();
}

// ------------------------------------------------------------------- //

void
TnmsSocket::resubscribe()
{}

// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //

bool
TnmsSocket::sendAuthRequest ( TnmsAuthRequest & request )
{
    return false;
}
// ------------------------------------------------------------------- //

bool
TnmsSocket::sendAuthReply ( TnmsAuthReply & reply )
{
    return false;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendMetric ( TnmsMetric & metric )
{
    return false;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendAdd ( TnmsAdd & metric )
{
    return false;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendRemove ( TnmsRemove & metric )
{
    return false;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::sendRequest ( TnmsRequest & metric )
{
    return false;
}

// ------------------------------------------------------------------- //

bool
TnmsSocket::setLastRecord()
{
    return false;
}

// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //
