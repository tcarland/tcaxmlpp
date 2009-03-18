#define _TNMSD_AGENTIOHANDLER_CPP_

#include "EventManager.h"

#include "AgentIOHandler.h"
#include "TnmsTree.h"
#include "TnmsClient.h"


namespace tnmsd {


AgentIOHandler::AgentIOHandler()
    : _tree(NULL)
{}

AgentIOHandler::AgentIOHandler ( TnmsTree * tree )
    : _tree(tree)
{}

AgentIOHandler::~AgentIOHandler() {}


void
AgentIOHandler::timeout ( const EventTimer * timer )
{
    //int  rd, wt;

}


void
AgentIOHandler::addMirrorConnection ( TnmsClient * client )
{
    _mirrors.insert(client);
}


void
AgentIOHandler::handle_accept ( const EventIO * io )
{
    Socket * svr = (Socket*) io->rock;
    BufferedSocket * sock = NULL;

    sock = (BufferedSocket*) svr->accept(BufferedSocket::factory);

    if ( sock == NULL )
        return;

    TnmsClient * client = new TnmsClient(_tree, sock);
    io->evmgr->addIOEvent(this, client->getSockFD(), (void*) client);

    // if compression; enable it
    // client->enableCompression();

    _clients.insert(client);
    return;
}

void
AgentIOHandler::handle_read ( const EventIO * io )
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
AgentIOHandler::handle_write ( const EventIO * io )
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
AgentIOHandler::handle_close ( const EventIO * io )
{
    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client == NULL )
        return;

    if ( client->isMirror() ) {
    } else {
        _clients.erase(client);
        _tree->removeClient(client);
    }

    client->close();
    io->evmgr->removeEvent(io->evid);

    return;
}


void
AgentIOHandler::handle_shut ( const EventIO * io )
{

}

void
AgentIOHandler::handle_destroy ( const EventIO * io )
{
    if ( io->isServer ) {
        Socket * svr = (Socket*) io->rock;
        if ( svr )
            delete svr;
    } else {
        TnmsClient * client = (TnmsClient*) io->rock;
        if ( client )
            delete client;
    }

    return;
}


bool
AgentIOHandler::readable ( const EventIO * io )
{
    return true;
}


bool
AgentIOHandler::writeable ( const EventIO * io )
{
    if ( io == NULL || io->isServer )
        return false;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client && client->getTxQueueSize() > 0 )
        return true;

    return false;
}


} // namespace 


/*  _TNMSD_AGENTIOHANDLER_CPP_  */

