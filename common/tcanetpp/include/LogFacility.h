/** 
  *  @file LogFacility.h
  *
  *    Logging facility supporting multiple, concurrent 
  *  logging i/o streams in a thread-safe manner. Supports syslog(unix), 
  *  file logging, or stdc++ iostream*
  *
  * Copyright (c) 2004,2008 Timothy Charlton Arland 
  * @author tca@charltontechnology.net
  *
  * @section LICENSE
  *
  * This file is part of tcanetpp.
  *
  * tcanetpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcanetpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcanetpp.  
  * If not, see <http://www.gnu.org/licenses/>.
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
# ifdef PTHREADS
#  include "ThreadLock.h"
# endif
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



/**  The LogFacility class is a convenient wrapper to various
 *   log mechanism's supporting thread-safe logging to syslog
 *   facilities, local file, and/or C++ I/O streams (std::ostream).
 **/
class LogFacility {

  public:

    typedef std::ostringstream   Message;


    static bool           InitThreaded    ( bool trylock = false );
    

    /*  open log methods */

    static bool           OpenLogFile     ( const std::string & logname,
                                            const std::string & prefix, 
                                            const std::string & filename, 
                                            bool append = true );
    
    static bool           OpenSyslog      ( const std::string & prefix, 
                                            int facility );
    
    static bool           OpenLogStream   ( const std::string & logname,
                                            const std::string & prefix,
                                            std::ostream * stream );
    
    
    static bool           AddLogStream    ( const std::string & logname, 
                                            const std::string & prefix,
                                            std::ostream * stream );
    
    /* close log methods */

    static std::ostream*  RemoveLogStream ( const std::string & logname,
                                            bool del  = false );

    static void           RemoveLogStreams( bool del  = false );
    

    static void           CloseSyslog();
    static std::ostream*  CloseLogFile    ( const std::string & logname, 
                                            bool del  = false );

    static void           CloseLogFacility();


    /*  log messages */
    
    static void           LogMessage      ( LogFacility::Message & logmsg, 
                                            int level = LOGFAC_NOTICE );

    static void           LogMessage      ( const std::string & entry, 
                                            int   level   = LOGFAC_NOTICE,
                                            bool  newline = true );

    static void           LogMessage      ( const std::string & logname,
                                            const std::string & entry,
                                            int   level   = LOGFAC_NOTICE, 
                                            bool  newline = true );


    static void           LogToAllStreams ( const std::string & entry,
                                            bool  newline = true );
   
    static void           LogToStream     ( const std::string & logname,
                                            const std::string & entry,
                                            bool  newline = true );

    /*  log settings */

    static bool           SetDefaultLogName   ( const std::string & name );
    static std::string    GetDefaultLogName();

    static void           SetDefaultLogPrefix ( const std::string & prefix );
    static std::string    GetDefaultLogPrefix() { return LogFacility::GetLogPrefix(); }

    static void           SetLogPrefix    ( const std::string & logname, 
                                            const std::string & prefix );
    static std::string    GetLogPrefix    ( const std::string & logname = "" );
    

    static bool           ShowLogTime     ( const std::string & logname, bool showTime );
    static void           SetLogTime      ( const time_t & now );
    static time_t         GetLogTime();

    static std::string    GetTimeString   ( const time_t & now );
    static int            GetDayOfYear    ( const time_t & now );

    static bool           SetEnabled      ( const std::string & logname, bool enable );
    static bool           GetEnabled      ( const std::string & logname, bool & enabled );

    static void           SetBroadcast    ( bool broadcast );

    static bool           IsOpen();
    static bool           IsOpen          ( const std::string & logname );

    static void           SetDebug        ( bool debug );
    static bool           GetDebug();

    static bool           RotateLogFile   ( const std::string & logname,
                                            const time_t & now );

private:

    static bool           InitSyslog      ( int facility );
    static void           InitLogMessage  ( const std::string & logname = "" );

    static bool           LogFacilityIsOpen();

    static bool           Lock();
    static void           Unlock();


private:

    struct LogStream 
    {
        std::string    logName;
        std::string    logPrefix;
        std::ostream*  logStream;
        int            today;
        bool           enabled;
        bool           showTime;

        LogStream ( std::ostream * strm = NULL )
            : logStream(strm),
              today(0),
              enabled(true),
              showTime(true)
        {
            if ( logStream == NULL )
                enabled = false;
        }

        LogStream ( const std::string & logName_,
                    const std::string & logPrefix_,
                    std::ostream      * strm )
            : logName(logName_),
              logPrefix(logPrefix_),
              logStream(strm),
              today(0),
              enabled(true),
              showTime(true)
        {
            if ( logStream == NULL )
                enabled = false;
        }
    };


    typedef std::map<std::string, LogStream>    StreamMap;


    static StreamMap             _StreamMap;

#   ifndef WIN32
#   ifdef PTHREADS
    static tcanetpp::ThreadLock  _Lock;
#   endif
#   endif

    static std::string           _LogName;
    static std::string           _LogPrefix;
    static std::string           _LogTimeStr;
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

