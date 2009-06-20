#define _TNMSCORE_AUTHCLIENT_CPP_

#include "tnmsCore.h"
#include "AuthClient.h"
#include "AuthIOHandler.h"

#include "LogFacility.h"
using namespace tcanetpp;



namespace tnmsCore {


AuthClient::AuthClient ( EventManager * evmgr ) 
    : _evmgr(evmgr),
      _evid(0),
      _authhandler(new AuthIOHandler()),
      _authport(0),
      _idleTimeout(0),
      _authRetryInterval(TNMS_LOGIN_INTERVAL),
      _bypass(false)
{}

AuthClient::AuthClient ( const std::string & authsvr, uint16_t authport, EventManager * evmgr )  
    : _evmgr(evmgr),
      _evid(0),
      _authhandler(new AuthIOHandler()),
      _authsvr(authsvr),
      _authport(authport),
      _idleTimeout(0),
      _authRetryInterval(TNMS_LOGIN_INTERVAL),
      _bypass(false)
{}

AuthClient::~AuthClient() 
{
    this->close();

    if ( _authhandler )
        delete _authhandler;
}


void 
AuthClient::timeout ( const EventTimer * timer )
{
    if ( _bypass )
        return;

    const time_t  & now = timer->abstime.tv_sec;
    const EventIO * io  = _evmgr->findIOEvent(_evid);

    if ( _authMap.size() == 0 && ! _authhandler->writeable(io) ) {
        if ( _idleTimeout > 0 && _idlet <= now ) {
            if ( this->isConnected() )
                this->close();
            _idlet = now + _idleTimeout;
        }
    } else {
        _idlet = now + _idleTimeout;
    }

    if ( ! this->isConnected() || this->isConnecting() )
        if ( this->connect() <= 0 )
            return;

    if ( this->receive(now) < 0 ) {
        this->close();
        return;
    }

    if ( this->send(now) < 0 ) {
        this->close();
        return;
    }

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

    if ( ! this->isConnected() || this->isConnecting() )
        if ( this->connect() < 0 )
            return false;

    rIter = _authMap.find(request.getElementName());
    if ( rIter == _authMap.end() )
        return false;

    AuthAttempt & proxy = rIter->second;

    LogFacility::LogMessage("AuthClient handling request for " + request.getElementName());

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

    rIter = _authMap.find(client->getClientLogin());

    if ( rIter != _authMap.end() )
        _authMap.erase(rIter);

    return;
}


int
AuthClient::connect()
{
    int conn = 0;

    if ( this->isConnected() )
        this->close();

    if ( (conn = this->openConnection(_authsvr, _authport)) < 0 )
        _errstr = "Failed to connect to " + this->getHostStr();
    else if ( conn > 0 )
        _evid = _evmgr->addIOEvent((EventIOHandler*) _authhandler, this->getFD(), this);

    return conn;
}

void
AuthClient::close()
{
    if ( _evmgr && _evid > 0 )
        _evmgr->removeEvent(_evid);

    _evid = 0;
    TnmsClient::close();
    return;
}

void
AuthClient::AuthReplyHandler ( TnmsAuthReply & reply ) 
{
    TnmsClient * client = NULL;
    AuthRequestMap::iterator  rIter;

    rIter = _authMap.find(reply.getElementName());

    if ( rIter == _authMap.end() ) {
        LogFacility::LogMessage("AuthClient::AuthReplyHandler() client not found: " + reply.getElementName());
        return;
    }

    LogFacility::LogMessage("AuthClient::AuthReplyHandler() " + reply.getElementName());

    client = rIter->second.client;

    if ( client ) {
        if ( client->sendMessage(&reply) ) {  // forward reply
            client->AuthReplyHandler(reply);
        } else {
            LogFacility::LogMessage("AuthClient::AuthReplyHandler() error forwarding reply to " + client->getHostStr());
            client->close();
        }
    }
    
    _authMap.erase(rIter);

    return;
}


void
AuthClient::setAuthServer ( const std::string & authsvr, uint16_t port )
{
    if ( _authsvr.compare(authsvr) !=0 ) {
        this->close();
        _authsvr = authsvr;
    }

    if ( _authport != port ) {
        this->close();
        _authport = port;
    }

    return;
}

} // namespace

