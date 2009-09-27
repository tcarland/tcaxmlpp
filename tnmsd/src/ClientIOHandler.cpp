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
    /*
    ClientSet::iterator  cIter;
    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter )
    {
        TnmsClient * client = (TnmsClient*) *cIter;

        if ( client && ! client->isMirror() )
            delete *cIter;
    }
    */
    _clients.clear();
}


void
ClientIOHandler::timeout ( const EventTimer * timer )
{
    int  rd  = 0;
    int  wt  = 0;

    const time_t & now = timer->abstime.tv_sec;

    ClientMap::iterator  cIter;
    for ( cIter = _clMap.begin(); cIter != _clMap.end(); ++cIter )
    {
        TnmsClient * client = (TnmsClient*) cIter->first;
        ClientStat & stat   = cIter->second;

        if ( client->isMirror() ) 
        {
            if ( ! client->isConnected() || client->isConnecting() )
            {
                int c = 0;

                if ( (stat.lastConn.getValue<time_t>() + client->getReconnectTime()) > now )
                    continue;

                if ( (c = client->connect()) < 0 ) {
                    LogFacility::LogMessage("ClientIOHandler mirror disconnected.");
                    continue;
                } else if ( c >= 0 ) {
                    timer->evmgr->addIOEvent(this, client->getSockFD(), client);
                }
                stat.connState.setValue(TNMS_INT32, c);
                stat.lastConn.setValue(TNMS_UINT32, now);
                stat.rxCtr.reset();
                stat.txCtr.reset();

                if ( c > 0 )
                    LogFacility::LogMessage("ClientIOHandler mirror connected " 
                        + client->getHostStr());
            }
            else 
            {
                if ( client->isAuthorized() && ! client->isSubscribed() ) 
                {
                    client->subscribeAll();
                } 
                else if ( ! client->isAuthorized() ) 
                {
                    if ( (stat.lastConn.getValue<time_t>() + client->getReconnectTime()) <= now ) {
                        client->login();
                        stat.lastConn.setValue(TNMS_UINT32, now);
                    }
                }
            }
        }

        if ( (rd = client->receive(now)) < 0 ) {
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

        this->updateStat(client, stat);
    }

    if ( _tree )
        _tree->updateSubscribers();

    return;
}

void
ClientIOHandler::addMirror ( TnmsClient * client )
{
    _clients.insert(client);
    this->initStat(client);
}


void
ClientIOHandler::eraseMirror ( TnmsClient * client )
{
    _clients.erase(client);
    this->endStat(client);
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

    LogFacility::LogMessage("ClientIOHandler::handle_accept() " + client->getHostStr());
    
    // if compression; enable it
    // client->enableCompression();
    _clients.insert(client);
    this->initStat(client);

    return;
}


void
ClientIOHandler::handle_read ( const EventIO * io )
{
    int  rd   = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (rd = client->receive(io->abstime.tv_sec)) < 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_read() error: " + client->getErrorStr());
        return this->handle_close(io);
    } else if ( rd > 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_read() " 
            + StringUtils::toString(client->getBytesReceived()));
    }
    
    return;
}


void
ClientIOHandler::handle_write ( const EventIO * io )
{
    int   wt  = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (wt = client->send(io->abstime.tv_sec)) < 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_write() error: " + client->getErrorStr());
        return this->handle_close(io);
    } else if ( wt > 0 ) {
        LogFacility::LogMessage("ClientIOHandler::handle_write() " 
            + StringUtils::toString(client->getBytesSent()));
    }

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
        _tree->removeSubscriber((TreeSubscriber*) client->getSubscriber());

    LogFacility::LogMessage("ClientIOHandler::handle_close() " + client->getHostStr());

    client->close();
    this->endStat(client);
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

void
ClientIOHandler::setPrefix ( const std::string & prefix )
{
    this->_prefix = prefix;
    // if ( clMap.size() > 0 )
    //   reset all of our metric names
}

void
ClientIOHandler::initStat ( TnmsClient * client )
{
    ClientStat    stat;
   
    std::string & name = stat.name;

    name = _prefix;
    name.append("/");

    if ( client->isMirror() )
        name.append(MIRROR_SUBNAME).append("/");
    else
        name.append(CLIENT_SUBNAME).append("/");

    name.append(client->getHostStr());

    stat.connState  = TnmsMetric(name);
    stat.lastConn   = TnmsMetric(name + "/" + LASTCONN_NAME);
    stat.rxCtr      = TnmsMetric(name + "/" + RECEIVE_NAME);
    stat.txCtr      = TnmsMetric(name + "/" + TRANSMIT_NAME);

    stat.connState.setValue(TNMS_INT32, (int)0);

    _tree->add(stat.connState.getElementName());
    _tree->add(stat.lastConn.getElementName());
    _tree->add(stat.rxCtr.getElementName());
    _tree->add(stat.txCtr.getElementName());

    _clMap[client] = stat;

    return;
}

void
ClientIOHandler::updateStat ( TnmsClient * client, ClientStat & stat )
{
    uint64_t  rx  = client->getBytesReceived();
    uint64_t  tx  = client->getBytesSent();

    stat.rxCtr.setValue(TNMS_UINT64, rx);
    stat.txCtr.setValue(TNMS_UINT64, tx);

    _tree->update(stat.connState);
    _tree->update(stat.lastConn);
    _tree->update(stat.rxCtr);
    _tree->update(stat.rxCtr);

    return;
}

void
ClientIOHandler::endStat ( TnmsClient * client )
{
    ClientMap::iterator cIter = _clMap.find(client);
    int c = -1;
    
    if ( cIter != _clMap.end() )
    {
        ClientStat & stat = cIter->second;

        if ( client->isMirror() ) {
            stat.connState.setValue(TNMS_INT32, c);
            stat.rxCtr.reset();
            stat.txCtr.reset();
            _tree->update(stat.connState);
        } else {
            _tree->remove(stat.name);
            _clMap.erase(cIter);
        } 
    }

    return;
}

} // namespace 


/*  _TNMSD_CLIENTIOHANDLER_CPP_  */

