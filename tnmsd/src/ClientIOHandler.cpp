#define _TNMSD_CLIENTIOHANDLER_CPP_

#include "ClientIOHandler.h"

#include "EventManager.h"
#include "LogFacility.h"


namespace tnmsd {


ClientIOHandler::ClientIOHandler ( TnmsTree * tree, AuthClient * auth ) 
    throw ( Exception )
    : _tree(tree),
      _auth(auth),
      _report(0)
{
    if ( NULL == _tree )
        throw Exception("ClientIOHandler() TnmsTree is NULL");
}


ClientIOHandler::~ClientIOHandler()
{
    _clients.clear();
    _clMap.clear();
}


void
ClientIOHandler::timeout ( const EventTimer & timer )
{
    int      rd  = 0;
    int      wt  = 0;
    int      c   = 0;
    uint32_t cnt = _clMap.size();

    const time_t & now = timer.abstime.tv_sec;

    ClientMap::iterator  cIter;
    for ( cIter = _clMap.begin(); cIter != _clMap.end(); ++cIter )
    {
        TnmsClient * client = (TnmsClient*) cIter->first;
        ClientStat & stat   = cIter->second;

        if ( timer.evid == _report ) {
            _numClients.setValue(TNMS_UINT32, cnt);
            _tree->update(_numClients);
            continue;
        }

        if ( client->isMirror() ) 
        {
            if ( ! client->isConnected() || client->isConnecting() )
            {
                if ( (stat.lastConn.getValue<time_t>() + client->getReconnectTime()) > now )
                    continue;

                if ( (c = client->connect()) < 0 ) {
                    LogFacility::LogMessage("ClientIOHandler mirror disconnected.");
                } else if ( c >= 0 ) {
                    LogFacility::LogMessage("ClientIOHandler mirror connected " 
                        + client->getHostStr());
                    timer.evmgr->addIOEvent(this, client->getSockFD(), client);
                }

                stat.connState.setValue(TNMS_INT32, c);
                stat.lastConn.setValue(TNMS_UINT32, now); // add ctime as metric data
                this->updateStat(client, stat);

                if ( c < 0 )
                    continue;
            }
            else 
            {
                if ( client->isAuthorized() && ! client->isSubscribed() ) {
                    client->subscribeAll();
                    c = 3;
                    stat.connState.setValue(TNMS_INT32, c);
                } else if ( ! client->isAuthorized() ) {
                    if ( (stat.lastConn.getValue<time_t>() + client->getReconnectTime()) <= now ) {
                        client->login();
                        stat.lastConn.setValue(TNMS_UINT32, now);
                    }
                }
                this->updateStat(client, stat);
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
    this->initStat(client);
}


void
ClientIOHandler::eraseMirror ( TnmsClient * client )
{
    _clients.erase(client);
    this->endStat(client);
}


void
ClientIOHandler::handle_accept ( const EventIO & io )
{
    Socket * svr = (Socket*) io.rock;
    BufferedSocket * sock = NULL;

    sock = (BufferedSocket*) svr->accept(BufferedSocket::factory);

    if ( sock == NULL ) 
        return;

    TnmsClient  * client = new TnmsClient(_tree, _auth, sock);
    io.evmgr->addIOEvent(this, client->getSockFD(), (void*) client);
    // if compression; enable it
    // client->enableCompression();
    _clients.insert(client);
    this->initStat(client);

    LogFacility::LogMessage("ClientIOHandler::handle_accept() " + client->getHostStr());
    
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

    if ( _tree ) 
        _tree->removeSubscriber((TreeSubscriber*) client->getSubscriber());

    LogFacility::LogMessage("ClientIOHandler::handle_close() " + client->getHostStr());

    client->close();
    _clients.erase(client);
    this->endStat(client);

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

void
ClientIOHandler::setPrefix ( const std::string & prefix )
{
    this->_prefix = prefix;

    std::string  name = prefix;
    uint32_t     cnt  = _clMap.size();
    
    name.append("/").append(CLIENT_SUBNAME);
    
    _numClients = TnmsMetric(name);
    _numClients.setValue(TNMS_UINT32, cnt);
}

void
ClientIOHandler::initStat ( TnmsClient * client )
{
    ClientStat    stat;
    int c = 0;
   
    std::string & name = stat.name;

    name = _prefix;
    name.append("/");

    if ( client->isMirror() ) {
        name.append(MIRROR_SUBNAME).append("/");
    } else {
        name.append(CLIENT_SUBNAME).append("/");
    }

    name.append(client->getAddrStr());
    name.append(":").append(StringUtils::toString(client->getHostPort()));

    stat.connState  = TnmsMetric(name);
    stat.lastConn   = TnmsMetric(name + "/" + LASTCONN_NAME);

    client->isMirror() ? c = 0 : c = 1;
    stat.connState.setValue(TNMS_INT32, c);

    _tree->add(stat.connState.getElementName());

    if ( client->isMirror() )
        _tree->add(stat.lastConn.getElementName());

    _clMap[client] = stat;

    return;
}

void
ClientIOHandler::updateStat ( TnmsClient * client, ClientStat & stat )
{
    _tree->update(stat.connState);
    if ( client->isMirror() )
        _tree->update(stat.lastConn);

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
            _tree->update(stat.connState);
        } else {
            _tree->remove(stat.name);
            _clMap.erase(cIter);
        } 
    }

    return;
}

void
ClientIOHandler::setReportEvent ( evid_t & report_id )
{
    _report = report_id;
}


} // namespace 


/*  _TNMSD_CLIENTIOHANDLER_CPP_  */

