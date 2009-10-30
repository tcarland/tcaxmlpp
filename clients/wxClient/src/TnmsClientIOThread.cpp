#define _TNMSCLIENTIOTHREAD_CPP_

#include "TnmsClientIOThread.h"
#include "TnmsClientIOHandler.h"

#include "LogFacility.h"
using namespace tcanetpp;


/**  ClientIOTimer */
void
TnmsClientIOThread::ClientIOTimer::timeout ( const EventTimer & timer )
{
    if ( this->iothread == NULL )
        return;

    return this->iothread->timeout(timer);
}

//------------------------------------------------------------------------------


TnmsClientIOThread::TnmsClientIOThread ( TnmsTree * tree, ThreadLock * rlock )
    : _evmgr(new tcanetpp::EventManager()),
      _tree(tree),
      _mutex(rlock)
{
    _clientHandler = new TnmsClientIOHandler(_mutex);
}


TnmsClientIOThread::~TnmsClientIOThread()
{
    delete _clientHandler;
    delete _evmgr;
}


void
TnmsClientIOThread::run()
{
    ClientIOTimer * timer = new ClientIOTimer(this);

    _evmgr->addTimerEvent(timer, 1, 0);
    _evmgr->eventLoop();

    return;
}

void
TnmsClientIOThread::timeout ( const EventTimer & timer )
{
    if ( this->_Alarm ) {
        _evmgr->setAlarm();
        _mutex->notify();
        return;
    }

    if ( ! this->_mutex->lock() )
        return;

    _tree->updateSubscribers();
    this->_clientHandler->timeout(timer);

    this->_mutex->unlock();

    return;
}


bool
TnmsClientIOThread::addClient ( TnmsClient * client )
{
    if ( client == NULL )
        return false;

    if ( ! this->_mutex->lock() )
        return false;

    evid_t  id = _evmgr->addIOEvent(_clientHandler, client->getDescriptor(), client);

    _clients[client] = id;
    _clientHandler->addClient(client);

    this->_mutex->unlock();

    return true;
}

void
TnmsClientIOThread::removeClient ( TnmsClient * client )
{
    ClientEventMap::iterator  cIter;

    if ( ! this->_mutex->lock() )
        return;

    cIter = _clients.find(client);
    if ( cIter != _clients.end() )
    {
        _evmgr->removeEvent(cIter->second);
        _clients.erase(cIter);
    }
    this->_mutex->unlock();

    return;
}



