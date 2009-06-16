#ifndef _TNMSCONSOLE_MANAGER_H_
#define _TNMSCONSOLE_MANAGER_H_


#include <string>
#include <map>

#include "EventManager.h"
#include "TnmsClient.h"
using namespace tcanetpp;
using namespace tnmsCore;

#include "ClientIOHandler.h"
#include "ConsoleThread.h"


namespace tnmsconsole {


typedef std::map<std::string, TnmsClient*>               ClientMap;



class TnmsConsoleManager : public EventTimerHandler {

public:

    TnmsConsoleManager ( std::istream & istrm, bool interactive, bool echo = false );
    virtual ~TnmsConsoleManager();


    virtual void timeout ( const EventTimer * timer );


    void         run();


    int          runConsole   ( std::istream & istrm,
                                bool           showprompt,
                                bool           echo = false );

    bool         createClient ( const std::string & name,
                                const std::string & host,
                                uint16_t            port );

    bool         closeClient  ( const std::string & name );
    
    void         runClientCommand  ( const std::string & cmd );

    void         displayHelp();
    void         setAlarm();
    void         setDebug ( bool d ) { _debug = d; }
    

private:

    EventManager*       _evmgr;
    TnmsTree*           _tree;
    ConsoleThread*      _console;

    ClientMap           _clients;
    ClientIOHandler*    _clientHandler;

    std::string         _user;
    std::string         _pw;

    bool                _debug;

};


}  // namespace

#endif  //  _TNMSCONSOLE_MANAGER_H_

