#define _TNMSCLIENTIOHANDLER_CPP_

#include "TnmsClientIOHandler.h"

#include "EventManager.h"
#include "LogFacility.h"
using namespace tcanetpp;


TnmsClientIOHandler::TnmsClientIOHandler ( ThreadLock * readLock ) 
    : _rlock(readLock)
{}

TnmsClientIOHandler::~TnmsClientIOHandler() {}


void
TnmsClientIOHandler::timeout ( const EventTimer * timer )
{
    int  rd  = 0;
    int  wt  = 0;

    const time_t & now = timer->abstime.tv_sec;

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
                    timer->evmgr->addIOEvent(this, client->getSockFD(), client);
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
                    client->subscribeAll();
                } else if ( ! client->isAuthorized() ) {
                    client->login();
                }
            }
        }

        if ( ! _rlock->lock() )
            return;

        rd = client->receive(now);
        
        _rlock->unlock();

        if ( rd < 0 ) {
            LogFacility::LogMessage("ClientIOHandler error in receive() from client " 
                + client->getHostStr());
            client->close();
            continue;
        }  // else report rd;


        if ( (wt = client->send(now)) < 0 ) {
            LogFacility::LogMessage("ClientIOHandler error in send() from client " 
                + client->getHostStr());
            client->close();
            continue;
        } // else report wt;

    }

    //if ( _tree )
        //_tree->updateClients();

    return;
}

void
TnmsClientIOHandler::addClient (TnmsClient * client )
{
    _clients.insert(client);
}


void
TnmsClientIOHandler::removeClient ( TnmsClient * client )
{
    _clients.erase(client);
}



void
TnmsClientIOHandler::handle_read ( const EventIO * io )
{
    int  rd   = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( ! _rlock->lock() )
        return;

    rd = client->receive(io->abstime.tv_sec);

    _rlock->unlock();

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
TnmsClientIOHandler::handle_write ( const EventIO * io )
{
    int   wt  = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (wt = client->send(io->abstime.tv_sec)) < 0 ) {
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
TnmsClientIOHandler::handle_close ( const EventIO * io )
{
    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client == NULL )
        return;

    LogFacility::LogMessage("ClientIOHandler::handle_close() " 
        + client->getHostStr());

    client->close();
    _clients.erase(client);
    io->evmgr->removeEvent(io->evid);

    return;
}


void
TnmsClientIOHandler::handle_destroy ( const EventIO * io )
{    
    LogFacility::LogMessage("ClientIOHandler::handle_destroy()");

    if ( io->isServer ) {
        Socket * svr = (Socket*) io->rock;
        if ( svr )
            delete svr;
    } else {
        TnmsClient * client = (TnmsClient*) io->rock;
        if ( client && ! client->isMirror() )
            delete client;
    }

    return;
}



bool
TnmsClientIOHandler::readable ( const EventIO * io )
{
    return true;
}


bool
TnmsClientIOHandler::writeable ( const EventIO * io )
{
    if ( io->isServer )
        return false;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client && client->txBytesBuffered() > 0 )
        return true;

    return false;
}


