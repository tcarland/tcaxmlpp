#ifndef _TNMSCONSOLE_CONSOLETHREAD_H_
#define _TNMSCONSOLE_CONSOLETHREAD_H_

#include <istream>
#include <string>
#include <map>

#include "Thread.h"
#include "ThreadLock.h"
#include "Queue.hpp"
using namespace tcanetpp;

#include "TnmsAPI.h"
using namespace tnmsApi;

namespace tnmsconsole {

typedef std::vector<std::string>                    CommandList;
typedef tcanetpp::SynchronizedQueue< CommandList >  CommandQueue;

typedef std::map<std::string, TnmsAPI*>             ApiMap;
typedef std::map<std::string, TnmsAPI*>::iterator   ApiIter;
typedef std::pair<ApiIter, bool>                    ApiMapInsert;



class ConsoleThread : public Thread {

public:

    ConsoleThread ( std::istream & istrm, bool interactive, bool echo );

    virtual ~ConsoleThread();


    virtual void run();


    bool         addClientCommand ( const CommandList & cmd );
    bool         getClientCommand ( CommandList & cmd );

    void         startClientProcessing();
    void         stopClientProcessing();

    static void  DisplayHelp();

    static bool  GetQuotedString ( size_t indx, CommandList & cmdlist, 
                                   std::string & str );

protected:

    int          sendAPIUpdates   ( TnmsAPI * api, const time_t & now );
    void         sleeps           ( int secs );


private:

    CommandQueue        _cmdQueue;
    ThreadLock          _lock;

    std::istream &      _istrm;
    bool                _prompt;
    bool                _echo;
    bool                _stop;

};



}  // namespace 


#endif  // _TNMSCONSOLE_CONSOLETHREAD_H_
