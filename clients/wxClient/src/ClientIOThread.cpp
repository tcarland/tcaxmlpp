#define _TNMSCLIENT_CLIENTIOTHREAD_CPP_

#include "ClientIOThread.h"
#include "ClientIOHandler.h"
#include "ClientTreeMutex.h"

#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsClient {


//------------------------------------------------------------------------------

//  ClientIOTimer
void
ClientIOThread::ClientIOTimer::timeout ( const EventTimer & timer )
{
    if ( this->iothread == NULL )
        return;

    return this->iothread->timeout(timer);
}

//------------------------------------------------------------------------------

ClientIOThread::ClientIOThread ( ClientTreeMutex * tree )
    : _evmgr(new tcanetpp::EventManager()),
      _mtree(tree),
      _clientHandler(new ClientIOHandler(_mtree))
{
}


ClientIOThread::~ClientIOThread()
{
    delete _clientHandler;
    delete _evmgr;
}


void
ClientIOThread::run()
{
    ClientIOTimer * timer = new ClientIOTimer(this);

    _evmgr->addTimerEvent(timer, 1, 0);
    _evmgr->eventLoop();

    return;
}


void
ClientIOThread::timeout ( const EventTimer & timer )
{
    TnmsTree * tree = NULL;

    if ( this->_Alarm ) {
        _evmgr->setAlarm();
        _mtree->notify();
        return;
    }

    if ( (tree = _mtree->acquireTree()) == NULL )
        return;

    tree->updateSubscribers();
    _clientHandler->timeout(timer);
    _mtree->releaseTree();

    return;
}


bool
ClientIOThread::addClient ( TnmsClient * client )
{
    if ( client == NULL )
        return false;

    if ( ! this->_mtree->lock() )
        return false;

    evid_t  id = _evmgr->addIOEvent(_clientHandler, client->getDescriptor(), client);

    _clients[client] = id;
    _clientHandler->addClient(client);

    this->_mtree->unlock();

    return true;
}


bool
ClientIOThread::removeClient ( TnmsClient * client )
{
    ClientEventMap::iterator  cIter;

    if ( ! this->_mtree->lock() )
        return false;

    cIter = _clients.find(client);
    if ( cIter != _clients.end() )
    {
        _evmgr->removeEvent(cIter->second);
        _clients.erase(cIter);
    }

    this->_mtree->unlock();

    return true;
}

} // namespace

// _TNMSCLIENT_CLIENTIOTHREAD_CPP_

