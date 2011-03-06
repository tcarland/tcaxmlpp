#define _TNMSCLIENT_CLIENTIOHANDLER_CPP_

#include "ClientIOHandler.h"
#include "ClientTreeMutex.h"

#include "EventManager.h"
#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsClient {


ClientIOHandler::ClientIOHandler ( ClientTreeMutex * tree )
    : _mtree(tree)
{}

ClientIOHandler::~ClientIOHandler() {}


void
ClientIOHandler::timeout ( const EventTimer & timer )
{
    int  rd  = 0;
    int  wt  = 0;

    const time_t & now = timer.abstime.tv_sec;

    ClientSet::iterator  cIter;
    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter )
    {
        TnmsClient * client = (TnmsClient*) *cIter;

        if ( client->isMirror() ) 
        {
            if ( ! client->isConnected() || client->isConnecting() )
            {
                int c = 0;

                if ( (c = client->connect()) < 0 ) {
                    LogFacility::LogMessage("ClientIOHandler mirror disconnected.");
                    continue;
                } else if ( c >= 0 ) {
                    timer.evmgr->addIOEvent(this, client->getSockFD(), client);
                    continue;
                }

                if ( c > 0 ) {
                    LogFacility::LogMessage("ClientIOHandler mirror connected " 
                        + client->getHostStr());
                    continue;
                }
            }
            else 
            {
                if ( client->isAuthorized() && ! client->isSubscribed() ) {
                    TnmsRequest reqTree("*", REQUEST_MESSAGE);
                    client->subscribeStructure();
                    client->subscribe("/");
                    client->sendMessage(&reqTree, true);
                } else if ( ! client->isAuthorized() ) {
                    if ( ! client->isAuthorizing() )
                        client->login();
                    else 
                        return;
                }
            }
        }

        rd = client->receive(now);
        
        if ( rd < 0 ) {
            LogFacility::LogMessage("ClientIOHandler error in receive() from client " 
                + client->getHostStr());
            client->close();
            continue;
        }

        if ( (wt = client->send(now)) < 0 ) {
            LogFacility::LogMessage("ClientIOHandler error in send() from client " 
                + client->getHostStr());
            client->close();
            continue;
        }

    }

    return;
}

void
ClientIOHandler::addClient (TnmsClient * client )
{
    _clients.insert(client);
}


void
ClientIOHandler::removeClient ( TnmsClient * client )
{
    _clients.erase(client);
}


void
ClientIOHandler::handle_read ( const EventIO & io )
{
    int  rd   = 0;

    if ( io.isServer )
        return;

    TnmsClient * client = (TnmsClient*) io.rock;

    if ( ! _mtree->lock() )
        return;

    rd = client->receive(io.abstime.tv_sec);

    _mtree->unlock();

    if ( rd < 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_read() error: " 
            + client->getErrorStr());
        return this->handle_close(io);
    } else if ( rd > 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_read() " 
            + StringUtils::toString(client->getBytesReceived()));
    }
    
    return;
}

void
ClientIOHandler::handle_write ( const EventIO & io )
{
    int   wt  = 0;

    if ( io.isServer )
        return;

    TnmsClient * client = (TnmsClient*) io.rock;

    if ( ! _mtree->lock() )
        return;
    
    wt = client->send(io.abstime.tv_sec);

    _mtree->unlock();

    if ( wt < 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_write() error: " 
            + client->getErrorStr());
        return this->handle_close(io);
    } else if ( wt > 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_write() " 
            + StringUtils::toString(client->getBytesSent()));
    }

    return;
}


void
ClientIOHandler::handle_close ( const EventIO & io )
{
    if ( io.isServer )
        return;

    TnmsClient * client = (TnmsClient*) io.rock;
    
    if ( client == NULL )
        return;

    LogFacility::LogMessage("ClientIOHandler::handle_close() " 
        + client->getHostStr());

    if ( ! _mtree->lock() )
        return;

    client->close();
    _clients.erase(client);
    io.evmgr->removeEvent(io.evid);

    _mtree->unlock();

    return;
}


void
ClientIOHandler::handle_destroy ( const EventIO & io )
{    
    LogFacility::LogMessage("ClientIOHandler::handle_destroy()");

    if ( io.isServer ) {
        Socket * svr = (Socket*) io.rock;
        if ( svr )
            delete svr;
    } else {
        TnmsClient * client = (TnmsClient*) io.rock;
        if ( client && ! client->isMirror() )
            delete client;
    }

    return;
}



bool
ClientIOHandler::readable ( const EventIO & io )
{
    return true;
}


bool
ClientIOHandler::writeable ( const EventIO & io )
{
    if ( io.isServer )
        return false;

    TnmsClient * client = (TnmsClient*) io.rock;

    if ( client && client->txBytesBuffered() > 0 )
        return true;

    return false;
}


} // namespace
