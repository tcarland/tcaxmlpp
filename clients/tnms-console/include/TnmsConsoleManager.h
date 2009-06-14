#ifndef _TNMSCONSOLE_MANAGER_H_
#define _TNMSCONSOLE_MANAGER_H_


#include <string>
#include <map>

#include "EventManager.h"
#include "TnmsClient.h"
using namespace tcanetpp;
using namespace tnmsCore;


namespace tnmsconsole {


typedef std::map<evid_t, TnmsClient*>   ClientMap;



class TnmsConsoleManager : public EventTimerHandler {

public:

    TnmsConsoleManager();
    virtual ~TnmsConsoleManager();

    virtual void timeout ( const EventTimer * timer );


    void         run();

    void         setAlarm();
    

private:

    EventManager*       _evmgr;
    TnmsTree*           _tree;

    ClientMap           _clients;
    ClientIOHandler*    _clientHandler;

};


}  // namespace

#endif  //  _TNMSCONSOLE_MANAGER_H_

