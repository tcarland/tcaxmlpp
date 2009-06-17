#ifndef _TNMSCONSOLE_CONSOLETHREAD_H_
#define _TNMSCONSOLE_CONSOLETHREAD_H_

#include <istream>
#include <string>
#include <map>

#include "Thread.h"
#include "Queue.hpp"
using namespace tcanetpp;

#include "TnmsAPI.h"
using namespace tnmsCore;

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


    bool         addClientCommand ( const std::string & cmd );
    bool         getClientCommand ( std::string & cmd );


    static void  DisplayHelp();


protected:

    int          sendAPIUpdates   ( TnmsAPI * api, const time_t & now );
    void         sleeps           ( int secs );

private:


    CommandQueue        _cmdQueue;

    std::istream &      _istrm;
    bool                _prompt;
    bool                _echo;

};



}  // namespace 


#endif  // _TNMSCONSOLE_CONSOLETHREAD_H_
