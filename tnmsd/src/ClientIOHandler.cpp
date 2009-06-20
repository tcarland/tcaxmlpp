#define _TNMSD_CLIENTIOHANDLER_CPP_

#include "ClientIOHandler.h"

#include "EventManager.h"
#include "LogFacility.h"


namespace tnmsd {


ClientIOHandler::ClientIOHandler ( TnmsTree * tree, AuthClient * auth ) 
    throw ( Exception )
    : _tree(tree),
      _auth(auth)
{
    if ( NULL == _tree )
        throw Exception("ClientIOHandler() TnmsTree is NULL");
}


ClientIOHandler::~ClientIOHandler()
{
    ClientSet::iterator  cIter;

    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter )
        delete *cIter;
}


void
ClientIOHandler::timeout ( const EventTimer * timer )
{
    int  rd  = 0;
    int  wt  = 0;

    const time_t & now = timer->abstime.tv_sec;

    std::set<TnmsClient*>::iterator  cIter;

    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter ) {
        TnmsClient * client = *cIter;

        if ( (rd = client->receive(now)) < 0 ) {
            client->close();
            continue;
        }
        if ( (wt = client->send(now)) < 0 ) {
            client->close();
            continue;
        }
    }

    if ( _tree )
        _tree->updateClients();

    return;
}


void
ClientIOHandler::handle_accept ( const EventIO * io )
{
    Socket * svr = (Socket*) io->rock;
    BufferedSocket * sock = NULL;

    sock = (BufferedSocket*) svr->accept(BufferedSocket::factory);

    if ( sock == NULL ) 
        return;

    TnmsClient  * client = new TnmsClient(_tree, _auth, sock);
    io->evmgr->addIOEvent(this, client->getSockFD(), (void*) client);

    // if compression; enable it
    // client->enableCompression();
    LogFacility::LogMessage("ClientIOHandler::handle_accept() " + client->getHostStr());
    _clients.insert(client);

    return;
}


void
ClientIOHandler::handle_read ( const EventIO * io )
{
    int  rd   = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (rd = client->receive(io->abstime.tv_sec)) < 0 )
        return this->handle_close(io);
    else if ( rd > 0 )
        LogFacility::LogMessage("ClientIOHandler::handle_read()" + StringUtils::toString(client->getBytesReceived()));
    
    return;
}


void
ClientIOHandler::handle_write ( const EventIO * io )
{
    int   wt  = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (wt = client->send(io->abstime.tv_sec)) < 0 )
        return this->handle_close(io);
    else if ( wt > 0 )
        LogFacility::LogMessage("ClientIOHandler::handle_write() " + StringUtils::toString(client->getBytesSent()));

    return;
}


void
ClientIOHandler::handle_close ( const EventIO * io )
{
    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client == NULL )
        return;

    if ( _tree ) 
        _tree->removeClient(client);

    LogFacility::LogMessage("ClientIOHandler::handle_close() " + client->getHostStr());

    client->close();
    _clients.erase(client);
    io->evmgr->removeEvent(io->evid);

    return;
}

void
ClientIOHandler::handle_shut ( const EventIO * io )
{

}

void
ClientIOHandler::handle_destroy ( const EventIO * io )
{
    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client == NULL )
        return;

    delete client;
}


bool
ClientIOHandler::readable ( const EventIO * io )
{
    return true;
}


bool
ClientIOHandler::writeable ( const EventIO * io )
{
    if ( io->isServer )
        return false;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client && client->txBytesBuffered() > 0 )
        return true;

    return false;
}



} // namespace 


/*  _TNMSD_CLIENTIOHANDLER_CPP_  */

