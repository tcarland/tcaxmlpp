#define _TNMSD_AGENTIOHANDLER_CPP_

#include "AgentIOHandler.h"

#include "EventManager.h"
#include "LogFacility.h"


namespace tnmsd {


AgentIOHandler::AgentIOHandler ( TnmsTree * tree, AuthClient * auth ) 
    throw ( Exception )
    : _tree(tree),
      _auth(auth)
{
    if ( NULL == _tree )
        throw Exception("AgentIOHandler() TnmsTree is NULL");
}

AgentIOHandler::~AgentIOHandler()
{
    ClientSet::iterator  cIter;
    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter )
        delete *cIter;
    _clients.clear();
}


void
AgentIOHandler::timeout ( const EventTimer * timer )
{
    int  rd  = 0;
    int  wt  = 0;
    
    const time_t & now = timer->abstime.tv_sec;

    ClientSet::iterator  cIter;
    for ( cIter = _clients.begin(); cIter != _clients.end(); cIter++ ) {
        TnmsClient * client = (TnmsClient*) *cIter;

        if ( (rd = client->receive(now)) < 0 ) {
            LogFacility::LogMessage("AgentIOHandler error in receive() " 
                + client->getErrorStr());
            client->close();
            continue;
        } else {
            // rd = ?
            ;
        }

        if ( (wt = client->send(now)) < 0 ) {
            LogFacility::LogMessage("AgentIOHandler error in send() " 
                + client->getErrorStr());
            client->close();
            continue;
        } else {
            // wt is our count
            ;
        }

    }

    return;
}


void
AgentIOHandler::handle_accept ( const EventIO * io )
{
    Socket * svr = (Socket*) io->rock;
    BufferedSocket * sock = NULL;

    sock = (BufferedSocket*) svr->accept(BufferedSocket::factory);

    if ( sock == NULL )
        return;

    TnmsClient * client = new TnmsClient(_tree, _auth, sock, true);
    evid_t id = io->evmgr->addIOEvent(this, client->getSockFD(), (void*) client);

    if ( id == 0 )
        LogFacility::LogMessage("AgentIOHandler::handle_accept() event error: " 
            + io->evmgr->getErrorStr());

    //if ( client->useCompression() )
    //    client->enableCompression();
    
    LogFacility::LogMessage("AgentIOHandler::handle_accept() " + client->getHostStr());
    _clients.insert(client);

    return;
}

void
AgentIOHandler::handle_read ( const EventIO * io )
{
    int  rd = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (rd = client->receive(io->abstime.tv_sec)) < 0 ) {
        LogFacility::LogMessage("AgentIOHandler::handle_read() error: " 
            + client->getErrorStr());
        return this->handle_close(io);
    } else if ( rd > 0 ) {
        LogFacility::LogMessage("AgentIOHandler::handle_read() bytes = " 
            + StringUtils::toString(client->getBytesReceived()));
    }

    return;
}

void
AgentIOHandler::handle_write ( const EventIO * io )
{
    int  wt = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (wt = client->send(io->abstime.tv_sec)) < 0 ) {
        LogFacility::LogMessage("AgentIOHandler::handle_write() error: " 
            + client->getErrorStr());
        return this->handle_close(io);
    } else if ( wt > 0 ) {
        LogFacility::LogMessage("AgentIOHandler::handle_write() bytes = " 
            + StringUtils::toString(client->getBytesSent()));
    }

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

    if ( _tree )
        _tree->removeClient(client);

    LogFacility::LogMessage("AgentIOHandler::handle_close() " + client->getHostStr());

    client->close();
    _clients.erase(client);
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
    LogFacility::LogMessage("AgentIOHandler::handle_destroy()");

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

    if ( client && client->txBytesBuffered() > 0 )
        return true;

    return false;
}


} // namespace 


//  _TNMSD_AGENTIOHANDLER_CPP_ 

