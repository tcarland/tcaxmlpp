#ifndef _TNMSCONSOLE_TNMSCONSOLEAPP_H_
#define _TNMSCONSOLE_TNMSCONSOLEAPP_H_

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
}

#include <string>
#include <map>
#include <vector>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/times.h>
#endif

#include "HexApp.h"
using namespace hexes;

#include "EventManager.h"

#include "TnmsAPI.h"
using namespace tnmsApi;
#include "tnmsCore.h"
using namespace tnmsCore;



namespace tnmsconsole {

#define MAX_SENDERR_CNT  5

class ClientTreeMutex;
class ClientIOThread;

typedef std::map<std::string, TnmsAPI*>    ApiMap;
typedef ApiMap::iterator                   ApiIter;
typedef std::pair<ApiIter, bool>           ApiMapInsert;
typedef std::vector<std::string>           CommandList;
typedef std::map<std::string, TnmsClient*> ClientMap;



class TnmsConsoleApp : public hexes::HexApp {

  public:

    TnmsConsoleApp();

    virtual ~TnmsConsoleApp();


    virtual void  run();

    void          setAlarm();

  protected:

    bool          processCmd       ( const std::string & cmd );
    void          processClientCmd ( CommandList & cmdlist );

    void          startClientProcessing();
    void          stopClientProcessing();

    bool          createClient     ( const std::string & name,
                                     const std::string & host,
                                     uint16_t            port );

    bool          removeClient     ( const std::string & name );
    void          listClients();


    int           sendAPIUpdates   ( TnmsAPI * api, const time_t & now );

    void          DisplayHelp();
    void          sleeps           ( int secs );

    static bool   GetQuotedString  ( size_t indx, CommandList & cmdlist, 
                                     std::string & str );


  private:

    HexPanel          * _mainPanel, *_statPanel, *_conPanel;

    ClientTreeMutex   * _mtree;
    ClientIOThread    * _iomgr;

    ApiMap              _apis;
    ApiMap::iterator    _showI;

    ClientMap           _clients;

    std::string         _title, _prompt;
    bool                _alarm, _stop;

};


} // namespace

#endif // _TNMSCONSOLE_TNMSCONSOLEAPP_H_

