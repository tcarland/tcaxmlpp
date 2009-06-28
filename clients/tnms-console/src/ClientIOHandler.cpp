#define _TNMSCONSOLE_CLIENTIOHANDLER_CPP_

#include "ClientIOHandler.h"

#include "EventManager.h"
#include "LogFacility.h"


namespace tnmsconsole {


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
    ClientMap::iterator  cIter;

    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter ) 
    {
        TnmsClient * client = (TnmsClient*) cIter->second;

        if ( client ) {
            if ( client->isConnected() )
                client->close();
            delete cIter->second;
        }
    }
    _clients.clear();
}


void
ClientIOHandler::timeout ( const EventTimer * timer )
{
    int  rd  = 0;
    int  sd  = 0;

    ClientSet::iterator  cIter;

    const time_t & now = timer->abstime.tv_sec;

    for ( cIter = _clientSet.begin(); cIter != _clientSet.end(); ++cIter ) {
        TnmsClient * client = *cIter;
        LogFacility::Message  logmsg;

        logmsg << "TnmsConsoleManager::timeout (" << client->getHostStr() << ")";

        if ( ! client->isConnected() || client->isConnecting() ) {
            int c = 0;

            if ( (c = client->connect()) < 0 ) {
                logmsg << " client disconnected." << std::endl;
                LogFacility::LogMessage(logmsg);
                continue;
            } else if ( c >= 0 ) {
                logmsg << " client (re)connecting..." << std::endl;
                LogFacility::LogMessage(logmsg);
                timer->evmgr->addIOEvent(this, client->getDescriptor(), client);
                continue;
            }

            if ( c > 0 ) {
                logmsg << " client connected." << std::endl;
                LogFacility::LogMessage(logmsg);
            }
        } else {
            if ( client->isAuthorized() && ! client->isSubscribed() ) {
                // suball?
                ;
            } else if ( ! client->isAuthorized() ) {
                client->login();
            }
        }


        if ( (rd = client->receive(now)) < 0 ) {
            client->close();
            continue;
        }

        if ( (sd = client->send(now)) < 0 ) {
            client->close();
            continue;
        }

    }

    if ( _tree )
        _tree->updateClients();

    return;
}



void
ClientIOHandler::handle_read ( const EventIO * io )
{
    int  rd  = 0;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (rd = client->receive(io->abstime.tv_sec)) < 0 )
        return this->handle_close(io);
    else if ( rd > 0 )
        LogFacility::LogMessage("ClientIOHandler::handle_read() bytes = " 
            + StringUtils::toString(rd));
    
    return;
}


void
ClientIOHandler::handle_write ( const EventIO * io )
{
    int  wt = 0;

    if ( io->isServer )
        return;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( (wt = client->send(io->abstime.tv_sec)) < 0 )
        return this->handle_close(io);
    else if ( wt > 0 )
        LogFacility::LogMessage("ClientIOHandler::handle_write() bytes = " 
            + StringUtils::toString(wt));

    return;
}


void
ClientIOHandler::handle_close ( const EventIO * io )
{
    if ( io->isServer )
        return;

    io->evmgr->removeEvent(io->evid);

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client == NULL )
        return;

    ClientSet::iterator  cIter = _clientSet.find(client);

    if ( cIter != _clientSet.end() ) {
        LogFacility::LogMessage("console", "Client close event handled for " + client->getHostStr());
        client->close();
    }

    if ( _tree ) 
        _tree->removeClient(client);
    
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
    
    ClientSet::iterator  cIter = _clientSet.find(client);

    if ( cIter == _clientSet.end() )
        delete client;

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


bool
ClientIOHandler::insert ( const std::string & name, TnmsClient * client )
{
    ClientMap::iterator  cIter;

    cIter = _clients.find(name);

    if ( cIter != _clients.end() )
        return false;

    _clients[name] = client;
    _clientSet.insert(client);

    return true;
}

void
ClientIOHandler::erase ( const std::string & name )
{
    ClientMap::iterator  cIter;

    cIter = _clients.find(name);

    if ( cIter != _clients.end() ) {
        cIter->second->close();
        _clientSet.erase(cIter->second);
        _clients.erase(cIter);
    }
    return;
}

ClientIOHandler::iterator
ClientIOHandler::begin()
{
    return _clients.begin();
}

ClientIOHandler::iterator
ClientIOHandler::end()
{
    return _clients.end();
}

ClientIOHandler::const_iterator
ClientIOHandler::begin() const
{
    return _clients.begin();
}

ClientIOHandler::const_iterator
ClientIOHandler::end() const
{
    return _clients.end();
}

size_t
ClientIOHandler::size() const
{
    return _clients.size();
}

void
ClientIOHandler::clear()
{
    _clients.clear();
    _clientSet.clear();
}

TnmsClient*
ClientIOHandler::find ( const std::string & name ) 
{
    TnmsClient * client = NULL;

    ClientMap::iterator  cIter;

    cIter = _clients.find(name);

    if ( cIter != _clients.end() )
        client = cIter->second;
    
    return client;
}

void
ClientIOHandler::listClients()
{
    ClientMap::iterator  cIter;

    LogFacility::LogMessage(" Client List ");
    
    for ( cIter = _clients.begin(); cIter != _clients.end(); ++cIter ) {
        LogFacility::Message  msg;

        msg << "    "  << cIter->first << "    =    " << cIter->second->getHostStr() << std::endl;

        LogFacility::LogMessage("console", msg.str());
    }

    return;
}


} // namespace 


/*  _TNMSCONSOLE_CLIENTIOHANDLER_CPP_  */

