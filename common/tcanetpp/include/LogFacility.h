/** 
  *   LogFacility - Logging facility supporting multiple syslog(unix), filelog, or
  *            log iostreams.
  * 
  *   @Author  tcarland@gmail.com 
 **/
#ifndef _TCANETPP_LOGFACILITY_H_
#define _TCANETPP_LOGFACILITY_H_

extern "C" {
#ifndef WIN32
# include <syslog.h>
#endif
}

#include <time.h>
#include <map>
#include <string>
#include <iosfwd>
#include <sstream>

#ifndef WIN32
# include "ThreadLock.h"
#endif

namespace tcanetpp {


typedef enum LogLevel
{
    LOGFAC_EMERG   = 0,
    LOGFAC_ALERT   = 1, 
    LOGFAC_ERR     = 3,
    LOGFAC_WARN    = 4,
    LOGFAC_NOTICE  = 5,
    LOGFAC_DEBUG   = 7
} logLevel_t;





class LogFacility {

  public:

    typedef std::ostringstream   Message;


    static bool           InitThreaded    ( bool trylock = false );
    

    static bool           OpenLogFile     ( const std::string & prefix,
                                            const std::string & filename, 
                                            bool append = true );
    
    static bool           OpenSyslog      ( const std::string & prefix, 
                                            int facility );
    
    static bool           OpenLogStream   ( const std::string & name, 
                                            std::ostream * stream );
    
    
    static bool           AddLogStream    ( const std::string & name, 
                                            std::ostream * stream );
    
    static std::ostream*  RemoveStream    ( const std::string & name );
    

    static void           CloseLogFacility();

    
    static void           LogMessage      ( const std::string & entry, 
                                            int level = LOGFAC_NOTICE );

    static void           LogMessage      ( const std::string & prefix, 
                                            const std::string & entry,
                                            int level = LOGFAC_NOTICE );

    static void           LogToStream     ( const std::string & streamName,
                                            const std::string & prefix,
                                            const std::string & entry );

    static void           LogToStream     ( const std::string & streamName,
                                            const std::string & entry );

    static void           LogToAllStreams ( const std::string & prefix,
                                            const std::string & entry );
                                        
    
    static void           CloseSyslog();
    static void           CloseFilelog();
    
    static void           SetLogTime      ( const time_t & now );
  
    static std::string    GetTimeString   ( const time_t & now );


    static void           SetEnabled      ( bool enable );
    static bool           GetEnabled();

    static void           SetBroadcast    ( bool broadcast );

    static void           SetLogPrefix    ( const std::string & prefix );
    static std::string    GetLogPrefix();

    static bool           IsOpen();

    static void           SetDebug        ( bool debug );
    static bool           GetDebug();

    static std::string    GetFileName();


private:

    typedef std::map<std::string, std::ostream*>  StreamMap;

    static bool           InitSyslog      ( int facility );
    static void           InitLogMessage();

    static bool           Lock();
    static void           Unlock();


private:

    static StreamMap             _StreamMap;

#   ifndef WIN32
    static tcanetpp::ThreadLock  _Lock;
#   endif

    static std::string           _LogPrefix;
    static std::string           _FileName;
    static std::string           _LogTimeStr;
    static std::ostream *        _LogStream;
    static time_t                _LogTime;
    static bool                  _Init;
    static bool                  _InitLock;
    static bool                  _TryLock;
    static bool                  _Enabled;
    static bool                  _Broadcast;
    static bool                  _Syslog;
    static bool                  _Debug;

};


} // namespace

#endif  // _TCANETPP_LOGFACILITY_H_

