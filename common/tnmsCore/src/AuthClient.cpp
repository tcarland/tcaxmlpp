#define _TNMSCORE_AUTHCLIENT_CPP_


#include "AuthClient.h"

using namespace tnmsCore {


AuthClient::AuthClient ( EventManager * evmgr ) 
    : _evmgr(evmgr),
      _evid(0),
      _authhandler(new AuthIOHandler())
{}

AuthCLient::~AuthClient()
{
    this->close();

    if ( _authhandler )
        delete _authhandler;
}


bool
AuthClient::authClient ( TnmsClient * client, TnmsAuthRequest & request )
{
    if ( client == NULL ) 
        return false;

    AuthAttempt  attempt;
    AuthRequestMap::iterator rIter;

    rIter = _authMap.find(request.getElementName());

    if ( rIter != _authMap.end() )
    {
        if ( rIter->second.sentat > 0 )
            return true;
    }

    attempt.request = request
    attempt.client  = client;

    _authMap[request.getElementName()]d = attempt;

    if ( ! this->isConnected() || this->isConnecting() )
        if ( this->connect() < 0 )
            return false;

    LogFacility::LogMessage("AuthClient handling request for " + request.getElementName());

    if ( this->sendMessage(request) )
        //sentat = now;
       ;
    
    return true;
}


void
AuthClient::unauthClient ( TnmsClient * client )
{
    if ( client == NULL ) 
        return;


    AuthRequestMap::iterator  rIter;

    rIter = _authMap.find(client.getClientLogin());

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
AuthClient::AuthReplyHandler ( const TnmsAuthReply & reply ) 
{
    TnmsClient * client = NULL;
    AuthRequestMap::iterator  rIter;


    rIter = _authMap.find(reply.getElementName());

    if ( rIter == _authMap.end() )
        return;

    LogFacility::LogMessage("AuthClient::AuthReplyHandler() " + reply.getElementName());

    client = rIter->second;

    if ( client ) {
        if ( client->sendMessage(reply) ) {  // forward reply
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
    if ( _authsvr.compare(authserver) !=0 ) {
        this->close();
        _authsvr = authsvr;
    }

    if ( _authport != port ) {
        this->close;
        _authport = port;
    }

    return;
}

