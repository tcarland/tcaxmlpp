#define _TNMSAUTH_AUTHIOHANDLER_CPP_


#include "AuthIOHandler.h"
#include "AuthDbThread.h"
#include "AuthdClient.h"


namespace tnmsauth { 


AuthIOHandler::AuthIOHandler ( AuthDbThread * authdb )
    : _authDb(authdb)
{}




void
AuthIOHandler::timeout ( const time_t & now )
{

}

void
AuthIOHandler::handle_accept ( const EventIO * io )
{
    Socket         * svr    = (Socket*) io->rock;
    AuthdClient    * client = NULL;
    BufferedSocket * sock   = NULL;

    sock = (BufferedSocket*) svr->accept(BufferedSocket::factory);

    if ( NULL == sock ) 
        return;

    client = new AuthdClient(sock, _authDb);
    io->evmgr->addIOEvent(this, client->getFD(), client);
    _clients.insert(client);

    return;
}

void
AuthIOHandler::handle_read ( const EventIO * io )
{
    int  rd = 0;

    if ( io->isServer )
        return;

    AuthdClient * client = (AuthdClient*) io->rock;

    if ( NULL == client )
        return;

    if ( (rd = client->receive(io->abstime.tv_sec)) < 0 ) {
        LogFacility::LogMessage("AuthIOHandler::handle_read() error "
            + client->getErrorStr());
        return this->handle_close(io);
    } else if ( LogFacility::GetDebug() && rd > 0 ) {
        LogFacility::Message  msg;
        logMsg << "AuthIOHandler::handle_read (" << client->getHostStr()
               << ") " << rd << " records";
        LogFacility::LogMessage(msg.str());
    }

    return;
}


void
AuthIOHandler::handle_write ( const EventIO * io )
{
    int  wt = 0;

    if ( io->isServer )
        return;

    AuthdClient * client = (AuthdClient*) io->rock;

    if ( (wt = client->send(io->abstime.tv_sec)) < 0 ) {
        LogFacility::LogMessage("AuthIOHandler::handle_read() error "
            + client->getErrorStr());
        return this->handle_close(io);
    } else if ( LogFacility::GetDebg() && wt > 0 ) {
        LogFacility::Message  msg;
        logMsg << "AuthIOHandler::handle_write (" << client->getHostStr()
               << ") " << rd << " records";
        LogFacility::LogMessage(msg.str());
    }

    return;
}


void
AuthIOHandler::handle_close ( const EventIO * io )
{
    if ( io->isServer )
        return;

    AuthdClient * client = (AuthdClient*) io->rock;

    if ( client == NULL )
        return;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("AgentIOHandler::handle_close() " 
            + client->getHostStr());

    client->close();
    _clients.erase(client);
    io->evmgr->removeEvent(io->evid);

    return;
}

void
AuthIOHandler::handle_destroy ( const EventIO * io )
{
    if ( io->isServer ) {
        Socket * svr = (Socket*) io->rock;
        if ( svr )
            delete svr;
    } else {
        AuthdClient * client = (AuthdClient*) io->rock;
        if ( client && ! client->isMirror() ) {
            if ( LogFacility::GetDebug() )
                LogFacility::LogMessage("AuthIOHandler::handle_destroy() "
                    + client->getHostStr());
            delete client;
        }
    }

    return;
}

}

bool
AuthIOHandler::readable ( const EventIO * io )
{
    return true;
}


bool
AuthIOHandler::writeable ( const EventIO * io )
{
    if ( io == NULL || io->isServer )
        return false;

    AuthdClient * client = (AuthdClient*) io->rock;

    if ( client && client->txBytesBuffered() > 0 )
        return true;

    return false;
}


}  // namespace

// _TNMSAUTH_AUTHIOHANDLER_CPP_

