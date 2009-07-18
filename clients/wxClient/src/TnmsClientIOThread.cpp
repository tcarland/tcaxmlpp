#define _TNMSCLIENTIOTHREAD_CPP_

#include "TnmsClientIOThread.h"
#include "TnmsClientIOHandler.h"

#include "LogFacility.h"
using namespace tcanetpp;


void
TnmsClientIOThread::ClientIOTimer::timeout ( const EventTimer * timer )
{
    if ( this->iothread == NULL )
        return;
    return this->iothread->timeout(timer);
}


TnmsClientIOThread::TnmsClientIOThread ( ThreadLock * rlock )
    : _evmgr(new tcanetpp::EventManager()),
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
TnmsClientIOThread::timeout ( const EventTimer * timer )
{
    //LogFacility::LogMessage("TnmsClientIOThread::timeout()");

    if ( this->_Alarm ) {
        _evmgr->setAlarm();
        _mutex->notify();
        return;
    }

    return this->_clientHandler->timeout(timer);
}


bool
TnmsClientIOThread::addClient ( TnmsClient * client )
{
    if ( client == NULL )
        return false;

    evid_t  id = _evmgr->addIOEvent(_clientHandler, client->getDescriptor(),
                    client);

    _clients[client] = id;
    _clientHandler->addClient(client);

    return true;
}

void
TnmsClientIOThread::removeClient ( TnmsClient * client )
{
    ClientEventMap::iterator  cIter;

    cIter = _clients.find(client);
    if ( cIter == _clients.end() )
        return;

    _evmgr->removeEvent(cIter->second);
    _clients.erase(cIter);

    return;
}
    


