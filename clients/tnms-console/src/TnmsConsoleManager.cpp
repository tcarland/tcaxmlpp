#define _TNMSCONSOLE_MANAGER_CPP_


#include "TnmsConsoleManager.h"

#include "LogFacility.h"



namespace tnmsconsole {



TnmsConsoleManager::TnmsConsoleManager()
    : _evmgr(new EventManager()),
      _tree(new TnmsTree())
{}

TnmsConsoleManager::~TnmsConsoleManager()
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

    delete _evmgr;
    delete _tree;
}


void
TnmsConsoleManager::run()
{
    LogFacility::OpenLogStream("stdout", &std::cout);
    LogFacility::LogMessage("TnmsConsoleManager::run()");
    LogFacility::SetBroadcast(true);
     
    _evmgr->addTimerEvent( (EventTimerHandler*) this, 5, 0);
    _evmgr->eventLoop();

    LogFacility::CloseLogFacility();

    return;
}



void
TnmsConsoleManager::timeout ( const EventTimer * timer )
{
    const time_t & now = timer->abstime.tv_sec;

    LogFacility::SetLogTime(now);


    ClientMap::iterator cIter;



}




TnmsConsoleManager::setAlarm()
{
    //_evmgr->setAlarm();
}

//TnmsConsoleManager::TnmsConsoleManager
//TnmsConsoleManager::TnmsConsoleManager
//TnmsConsoleManager::TnmsConsoleManager
//TnmsConsoleManager::TnmsConsoleManager
//TnmsConsoleManager::TnmsConsoleManager

}  // namespace


// _TNMSCONSOLE_MANAGER_CPP_

