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
{}

void
ClientIOHandler::timeout ( const EventTimer * timer )
{
    int  rd  = 0;
    int  wt  = 0;

    std::set<TnmsClient*>::iterator  cIter;

    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter ) {
        TnmsClient * client = *cIter;

        if ( (rd = client->receive()) < 0 ) {
            client->close();
            continue;
        }

        if ( (wt = client->send()) < 0 ) {
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

    _clients.insert(client);
    return;
}


void
ClientIOHandler::handle_read ( const EventIO * io )
{
    int  rd  = 0;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (rd = client->receive()) < 0 )
        return this->handle_close(io);
    
    return;
}


void
ClientIOHandler::handle_write ( const EventIO * io )
{
    int  wt = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (wt = client->send()) < 0 )
        return this->handle_close(io);

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
    _clients.erase(client);
    client->close();
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

