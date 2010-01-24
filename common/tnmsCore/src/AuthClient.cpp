#define _TNMSCORE_AUTHCLIENT_CPP_

#include "tnmsCore.h"
#include "AuthClient.h"

#include "LogFacility.h"
using namespace tcanetpp;



namespace tnmsCore {


// ---------------------------------------------------------------------------------------
//  AuthMessageHandler
AuthMessageHandler::AuthMessageHandler ( AuthClient * client )
    : _client(client)
{}

void
AuthMessageHandler::AuthReplyHandler ( const TnmsAuthReply & reply )
{
    if ( _client )
        _client->AuthReplyHandler(reply);
}

void
AuthMessageHandler::PingHandler()
{
    if ( _client )
    {
        TnmsMessage  ping(PING_REPLY);
        _client->sendMessage(&ping, true);
    }
}

// ---------------------------------------------------------------------------------------
//  AuthIOHandler

void
AuthIOHandler::handle_read ( const EventIO & io )
{
    TnmsClient * client = (TnmsClient*) io.rock;

    if  ( client && client->receive(io.abstime.tv_sec) < 0 ) {
        LogFacility::LogMessage("AuthIOHandler::handle_read() error: " + client->getHostStr());
        client->close();
    }

    return;
}

void
AuthIOHandler::handle_write ( const EventIO & io )
{
    TnmsClient * client = (TnmsClient*) io.rock;

    if  ( client && client->send(io.abstime.tv_sec) < 0 ) {
        LogFacility::LogMessage("AuthIOHandler::handle_write() error: " + client->getHostStr());
        client->close();
    }

    return;
}

void
AuthIOHandler::handle_close ( const EventIO & io )
{
    TnmsClient * client = (TnmsClient*) io.rock;

    if ( client ) {
        LogFacility::LogMessage("AuthIOHandler::handle_close() " + client->getHostStr());
        client->close();
    }

    return;
}

bool
AuthIOHandler::readable ( const EventIO & io )
{
    return true;
}

bool
AuthIOHandler::writeable ( const EventIO & io )
{
    TnmsClient * client = (TnmsClient*) io.rock;

    if ( client && client->txBytesBuffered() > 0 )
        return true;

    return false;
}

// ---------------------------------------------------------------------------------------
//  AuthClient

/** Need to reset the undlerlying default MessageHandler to hold 
  * our AuthClient*, not a TnmsClient*, so it must be reset in the
  * constructor to get our hook for AuthReplyHandler.
 **/
AuthClient::AuthClient ( EventManager * evmgr ) 
    : _evmgr(evmgr),
      _evid(0),
      _authhandler(new AuthIOHandler()),
      _idleTimeout(0),
      _authRetryInterval(TNMS_LOGIN_INTERVAL),
      _lastConn(0),
      _bypass(false)
{
    this->setMessageHandler(new AuthMessageHandler(this));
}

AuthClient::AuthClient ( const std::string & authsvr, uint16_t authport, 
                         EventManager * evmgr )  
    : _evmgr(evmgr),
      _evid(0),
      _authhandler(new AuthIOHandler()),
      _idleTimeout(0),
      _authRetryInterval(TNMS_LOGIN_INTERVAL),
      _lastConn(0),
      _bypass(false)
{
    this->setMessageHandler(new AuthMessageHandler(this));
    this->openConnection(authsvr, authport);
}

AuthClient::~AuthClient() 
{
    this->close();

    if ( _authhandler )
        delete _authhandler;
}


void 
AuthClient::timeout ( const EventTimer & timer )
{
    if ( _bypass || this->getHostname().empty() )
        return;

    const time_t  & now = timer.abstime.tv_sec;

    if ( _idleTimeout > 0 && _authMap.size() == 0 )
    {
        const EventIO * io  = _evmgr->findIOEvent(_evid);
        if ( io && ! _authhandler->writeable(*io) && _idlet <= now ) {
            if ( this->isConnected() )
                this->close();
            _idlet = now + _idleTimeout;
            return;
        }
    } 
    else 
    {
        _idlet = now + _idleTimeout;
    }

    if ( ! this->isConnected() || this->isConnecting() )
    {
        if ( (_lastConn + this->getReconnectTime()) > now )
            return;
        if ( this->authConnect() <= 0 ) {
            _lastConn = now;
            return;
        }
    }

    if ( ! this->isAuthorized() )
    {
        if ( this->isAuthorizing() && (_lastConn + (this->getReconnectTime() * 4)) > now )
            return;
        _lastConn = now;
        if ( this->login() )
            LogFacility::LogMessage("AuthClient validating auth channel");
    }

    if ( this->receive(now) < 0 ) {
        this->close();
        return;
    }

    if ( this->send(now) < 0 ) {
        this->close();
        return;
    }

    if ( ! this->isAuthorized() )
        return;

    AuthRequestMap::iterator  rIter;

    for ( rIter = _authMap.begin(); rIter != _authMap.end(); ++rIter ) {
        AuthAttempt & authp = rIter->second;

        if ( authp.request.getElementName().empty() || authp.client == NULL )
            continue;

        if ( authp.sent_at == 0 ) {
            if ( this->sendMessage(&authp.request) )
                authp.sent_at = now;
        } else if ( (authp.sent_at + _authRetryInterval) <= now ) {
            if ( this->sendMessage(&authp.request) )
                authp.sent_at = now;
        }
    }

    return;
}


bool
AuthClient::authClient ( TnmsClient * client, TnmsAuthRequest & request )
{
    if ( client == NULL ) 
        return false;

    LogFacility::LogMessage("AuthClient handling request for " + request.getElementName());

    if ( _bypass ) {
        TnmsAuthReply  reply(request.getElementName());
        reply.authResult(AUTH_SUCCESS);
        if ( client->sendMessage(&reply) ) {
            client->AuthReplyHandler(reply);
            return true;
        }
    }

    AuthAttempt    authp;
    AuthRequestMap::iterator rIter;

    rIter = _authMap.find(request.getElementName());
    if ( rIter != _authMap.end() )
    {
        if ( rIter->second.sent_at > 0 )
            return true;
    }

    authp.request = request;
    authp.client  = client;

    _authMap[request.getElementName()] = authp;

    rIter = _authMap.find(request.getElementName());
    if ( rIter == _authMap.end() )
        return false;

    if ( ! this->isAuthorized() )
        return false;

    AuthAttempt & proxy = rIter->second;

    // sent_at need not be accurate, can retrieve log time
    // instead of call to time?
    if ( this->sendMessage(&proxy.request) )
        proxy.sent_at = ::time(NULL);
    
    return true;
}


void
AuthClient::unauthClient ( TnmsClient * client )
{
    if ( client == NULL ) 
        return;

    AuthRequestMap::iterator  rIter;

    rIter = _authMap.find(client->getClientAuthStr());

    if ( rIter != _authMap.end() )
        _authMap.erase(rIter);

    return;
}


int
AuthClient::authConnect()
{
    int conn = 0;

    if ( this->isConnecting() ) {
        conn = this->openConnection();
    } else {
        if ( _evid > 0 )
            this->close();
        conn = this->openConnection();
    }

    if ( conn < 0 ) 
    {
        _errstr = "Failed to connect to " + this->getHostStr();
        this->close();
    }
    else if ( conn >= 0 && _evid == 0 )
    {
        _evid = _evmgr->addIOEvent((EventIOHandler*) _authhandler, this->getFD(), this);
    }

    return conn;
}

void
AuthClient::close()
{
    if ( _evmgr && _evid > 0 )
        _evmgr->removeEvent(_evid);

    _evid = 0;

    TnmsClient::close();
    LogFacility::LogMessage("AuthClient connection closed");

    return;
}

void
AuthClient::AuthReplyHandler ( const TnmsAuthReply & reply ) 
{
    TnmsClient * client = NULL;
    AuthRequestMap::iterator  rIter;

    rIter = _authMap.find(reply.getElementName());

    if ( rIter == _authMap.end() ) {
        if ( reply.getElementName().compare(this->getClientLoginName()) == 0 )
        {
            LogFacility::LogMessage("AuthClient::AuthReplyHandler() for server auth channel");
            TnmsClient::AuthReplyHandler(reply);
            return;
        }
        LogFacility::LogMessage("AuthClient::AuthReplyHandler() client not found: " 
            + reply.getElementName());
        return;
    }

    LogFacility::LogMessage("AuthClient::AuthReplyHandler() " + reply.getElementName());

    client = rIter->second.client;

    if ( client ) {
        if ( client->sendMessage(&reply, true) ) {  // forward reply
            client->AuthReplyHandler(reply);
        } else {
            LogFacility::LogMessage("AuthClient::AuthReplyHandler() error forwarding reply to " 
                + client->getHostStr());
            client->close();
        }
    }
    
    _authMap.erase(rIter);

    return;
}


void
AuthClient::setAuthServer ( const std::string & authsvr, uint16_t port )
{
    std::string  svrname = this->getHostname();
    uint16_t     svrport = this->getHostPort();
    
    if ( svrname.compare(authsvr) != 0 || svrport != port ) 
    {
        this->close();
        this->openConnection(authsvr, port);

        LogFacility::Message  logmsg;

        logmsg << "AuthClient (re)setting Auth Server to " 
               << authsvr << ":" << port;

        LogFacility::LogMessage(logmsg.str());
    }

    return;
}


} // namespace


