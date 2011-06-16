#define _TNMSDB_CLIENTIOHANDLER_CPP_

#include "ClientIOHandler.h"

#include "EventManager.h"
#include "LogFacility.h"


namespace tnmsdb {


ClientIOHandler::ClientIOHandler() 
    : _recon(0)
{ }


ClientIOHandler::~ClientIOHandler()
{
    _clients.clear();
}


void
ClientIOHandler::timeout ( const EventTimer & timer )
{
    int      rd  = 0;
    int      wt  = 0;
    int      c   = 0;

    const time_t & now = timer.abstime.tv_sec;

    ClientSet::iterator  cIter;
    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter )
    {
        TnmsClient * client = (TnmsClient*) *cIter;

        if ( ! client->isConnected() || client->isConnecting() )
        {
            if ( (_recon + client->getReconnectTime()) > now )
                continue;

            if ( (c = client->connect()) < 0 ) {
                LogFacility::LogMessage("ClientIOHandler mirror disconnected.");
            } else if ( c >= 0 ) {
                LogFacility::LogMessage("ClientIOHandler mirror connected " 
                    + client->getHostStr());
                timer.evmgr->addIOEvent(this, client->getSockFD(), client);
            }

            _recon = now;

            if ( c < 0 )
                continue;
        }
        else 
        {
            if ( client->isAuthorized() && ! client->isSubscribed() ) {
                LogFacility::LogMessage("ClientIOHandler authorized, sending resub");
                client->resubscribe();
            } else if ( ! client->isAuthorized() ) {
                if ( (_recon + client->getReconnectTime()) <= now ) {
                    LogFacility::LogMessage("ClientIOHandler sending login request");
                    client->login();
                    _recon = now;
                }
            }
        }

        if ( (wt = client->send(now)) < 0 ) {
            LogFacility::LogMessage("ClientIOHandler error in send() from client " 
                + client->getHostStr());
            client->close();
            continue;
        }

        if ( (rd = client->receive(now)) < 0 ) {
            LogFacility::LogMessage("ClientIOHandler error in receive() from client " 
                + client->getHostStr());
            client->close();
            continue;
        } 

    }

    return;
}

void
ClientIOHandler::addMirror ( TnmsClient * client )
{
    _clients.insert(client);
}


void
ClientIOHandler::eraseMirror ( TnmsClient * client )
{
    _clients.erase(client);
}


void
ClientIOHandler::handle_accept ( const EventIO & io )
{
    return;
}


void
ClientIOHandler::handle_read ( const EventIO & io )
{
    int  rd   = 0;

    if ( io.isServer )
        return;

    TnmsClient * client = (TnmsClient*) io.rock;

    if ( (rd = client->receive(io.abstime.tv_sec)) < 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_read() error: " + client->getErrorStr());
        return this->handle_close(io);
    } else if ( rd > 0 && LogFacility::GetDebug() ) {
        LogFacility::Message  logmsg;
        logmsg << "ClientIOHandler::handle_read (" << client->getHostStr()
            << "): " << client->getBytesReceived() << " bytes";
        LogFacility::LogMessage(logmsg.str());
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

    if ( (wt = client->send(io.abstime.tv_sec)) < 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_write() error: " + client->getErrorStr());
        return this->handle_close(io);
    } else if ( wt > 0 && LogFacility::GetDebug() ) {
        LogFacility::Message  logmsg;
        logmsg << "ClientIOHandler::handle_write (" << client->getHostStr()
            << "): " << client->getBytesSent() << " bytes";
        LogFacility::LogMessage(logmsg.str());
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

    LogFacility::LogMessage("ClientIOHandler::handle_close() " + client->getHostStr());

    client->close();
    //_clients.erase(client);

    io.evmgr->removeEvent(io.evid);

    return;
}


void
ClientIOHandler::handle_shut ( const EventIO & io )
{
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


/*  _TNMSDB_CLIENTIOHANDLER_CPP_  */

