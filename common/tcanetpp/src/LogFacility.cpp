/**
  *  LogFacility
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
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
#define _TCANETPP_LOGFACILITY_CPP_

#include <list>
#include <memory>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "LogFacility.h"

#ifdef WIN32
# pragma warning (disable:4996)
#endif

namespace tcanetpp {


// ----------------------------------------------------------------------
//  Initialization

LogFacility::StreamMap  LogFacility::_StreamMap  = LogFacility::StreamMap();

#ifndef WIN32
#ifdef PTHREADS
tcanetpp::ThreadLock  LogFacility::_Lock         = tcanetpp::ThreadLock();
#endif
#endif


time_t             LogFacility::_LogTime    = 0;
bool               LogFacility::_Init       = false;
bool               LogFacility::_InitLock   = false;
bool               LogFacility::_TryLock    = false;
bool               LogFacility::_Syslog     = false;
bool               LogFacility::_Broadcast  = false;
bool               LogFacility::_Enabled    = false;
bool               LogFacility::_Debug      = false;
std::string        LogFacility::_LogName    = "";
std::string        LogFacility::_LogPrefix  = "";
std::string        LogFacility::_LogTimeStr = "";


// ----------------------------------------------------------------------

/**  Initializes a thread-safe version of the LogFacility.
  *  If @param trylock is true, the mutex lock is non-blocking and if the
  *  mutex is not available, the various log functions will potentially
  *  return without logging the message.
 **/
bool
LogFacility::InitThreaded ( bool trylock )
{
#   ifdef WIN32
    return false;
#   else
    LogFacility::_TryLock  = trylock;
    LogFacility::_InitLock = true;
#   endif
    return true;
}

// ----------------------------------------------------------------------

bool
LogFacility::OpenLogFile ( const std::string & logname, 
                           const std::string & prefix,
                           const std::string & filename,
                           bool append )
{
    std::ofstream       * fstrm = NULL;
    StreamMap::iterator   sIter;

    sIter = LogFacility::_StreamMap.find(logname);
    
    if ( sIter != LogFacility::_StreamMap.end() )
        return false;

    std::auto_ptr<std::ofstream>  newfstrm(new std::ofstream());

    if ( append )
        newfstrm->open( filename.c_str(), std::ios::out | std::ios::app );
    else
        newfstrm->open( filename.c_str(), std::ios::out | std::ios::trunc );

    if ( ! newfstrm->is_open() )
        return false;

    fstrm  = newfstrm.release();

    LogFacility::AddLogStream(logname, prefix, (std::ostream*) fstrm);
    LogFacility::InitLogMessage(logname);

    return fstrm->is_open();
}


// ----------------------------------------------------------------------


bool
LogFacility::OpenSyslog ( const std::string & prefix, int facility )
{
#   ifndef WIN32
    if ( LogFacility::_Syslog )
        LogFacility::CloseSyslog();

    ::openlog(prefix.c_str(), LOG_PID, facility);
    LogFacility::_Syslog   = true;
    LogFacility::_Enabled  = true;
    LogFacility::InitLogMessage();

    return true;
#endif
    return false;
}


// ----------------------------------------------------------------------


bool
LogFacility::OpenLogStream ( const std::string & logname, const std::string & prefix, std::ostream * stream )
{
     return(LogFacility::AddLogStream(logname, prefix, stream));
}


// ----------------------------------------------------------------------


bool
LogFacility::AddLogStream ( const std::string & logname, const std::string & prefix, std::ostream * stream )
{
    bool result = false;

    if ( stream == NULL ||  ! LogFacility::Lock() )
        return result;

    StreamMap::iterator  sIter = LogFacility::_StreamMap.find(logname);

    if ( sIter == LogFacility::_StreamMap.end() ) {
        LogStream  lstrm(logname, prefix, stream);
        LogFacility::_StreamMap[logname] = lstrm;
        LogFacility::_Enabled = true;
        result = true;
    }

    LogFacility::Unlock();

    return result;
}


std::ostream*
LogFacility::RemoveLogStream ( const std::string & logname, bool del )
{
    std::ostream        *ptr = NULL;
    StreamMap::iterator  sIter;

    if ( ! LogFacility::Lock() )
        return ptr;

    sIter = LogFacility::_StreamMap.find(logname);
    if ( sIter != LogFacility::_StreamMap.end() ) {
        if ( del && sIter->second.logStream )
            delete sIter->second.logStream;
        else
            ptr = sIter->second.logStream;
        LogFacility::_StreamMap.erase(sIter);
    }

    if ( _StreamMap.empty() && ! _Syslog )
        _Enabled = false;

    LogFacility::Unlock();

    return ptr;
}

/**  Removes all streams from the LogFacility.
 *
 *   @param del  indicates whether the function should also try to
 *   'delete' the underlying log streams.  This can be dangerous
 *   and is generally not recommended. The better method when managing
 *   multiple logstreams in one application (rare), is to track the
 *   logname and remove them individually taking care to handle the
 *   associated log stream appropriately.  For instance, adding
 *   std::cerr as a log stream and then calling this function with
 *   del = true would cause a segfault.
 */
void
LogFacility::RemoveLogStreams ( bool del )
{
    StreamMap::iterator     sIter;

    if ( ! LogFacility::Lock() )
        return;

    for ( sIter = _StreamMap.begin(); sIter != _StreamMap.end(); ++sIter ) {
        if ( sIter->second.logStream == NULL || sIter->second.logStream == &std::cout )
            continue;
        
        if ( del )
            delete sIter->second.logStream;
    }

    _StreamMap.clear();

    if ( ! _Syslog )
        _Enabled = false;

    LogFacility::Unlock();

    return;
}


// ----------------------------------------------------------------------

/*  Returns the current state of the entire LogFacility. If no log streams
 *  exist or are currently 'enabled' this will return false, otherwise true
 *  if any one or more log streams are enabled
 */
bool
LogFacility::LogFacilityIsOpen()
{
    StreamMap::const_iterator  sIter;

    for ( sIter = _StreamMap.begin(); sIter != _StreamMap.end(); ++sIter )
        if ( sIter->second.enabled )
            return true;

    return false;
}


/**  Set the log state of an existing log facility. */
bool
LogFacility::SetEnabled ( const std::string & logname, bool enabled )
{
    if ( ! LogFacility::Lock() )
        return false;

    StreamMap::iterator  sIter = LogFacility::_StreamMap.find(logname);
    if ( sIter != _StreamMap.end() )
        sIter->second.enabled = enabled;

    if ( ! enabled && _StreamMap.size() > 1 )
        LogFacility::_Enabled = LogFacility::LogFacilityIsOpen();

    LogFacility::Unlock();

    return true;
}


/**  There are two conditions under which this function can fail. If
 *   the underlying mutex fails or the @param logname as provided does
 *   not exist. For this reason, the actual boolean indicating whether
 *   an existing stream is 'enabled' is a pass-by-value parameter.
 */
bool
LogFacility::GetEnabled ( const std::string & logname, bool & enabled )
{
    bool result = false;

    if ( ! LogFacility::Lock() )
        return result;

    StreamMap::iterator  sIter = LogFacility::_StreamMap.find(logname);
    if ( sIter != _StreamMap.end() ) {
        enabled = sIter->second.enabled;
        result  = true;
    }

    LogFacility::Unlock();

    return result;
}

/**  Sets the current 'broadcast' state.  Messages can be intentionally
 *   broadcast using the 'LogToAllStreams' function. Alternatively,
 *   broadcasting can be explicitly forced for all logging by all threads
 *   by setting the broadcast state to true via this function.
 */
void
LogFacility::SetBroadcast ( bool broadcast )
{
    LogFacility::_Broadcast = broadcast;
}

// ----------------------------------------------------------------------

void
LogFacility::SetDebug ( bool d )
{
    LogFacility::_Debug = d;
}

bool
LogFacility::GetDebug()
{
    return LogFacility::_Debug;
}

bool
LogFacility::IsOpen()
{
    bool open = false;

    if ( LogFacility::_StreamMap.size() == 0 )
        return open;

    if ( ! LogFacility::Lock() )
        return open;

    StreamMap::iterator sIter;
    for ( sIter = _StreamMap.begin(); sIter != _StreamMap.end(); ++sIter )
    {
        if ( sIter->second.enabled ) {
            open = true;
            break;
        }
    }

    LogFacility::Unlock();
    
    return open;
}

bool
LogFacility::IsOpen ( const std::string & logname ) 
{
    bool open = false;

    LogFacility::GetEnabled(logname, open);

    return open;
}
        

// ----------------------------------------------------------------------

/**  Logs a message via the LogFacility::Message object to the default or
 *   primary LogFacility as defined by 'SetDefaultLogName' function.
 *
 *   Note that level is validated via this call so messages logged as the
 *   LOGFAC_DEBUG log level will only really log if the
 *   LogFacility::SetDebug(true) has be set when NOT using syslog.
 */
void
LogFacility::LogMessage ( LogFacility::Message & logmsg, int level )
{
    return LogFacility::LogMessage(LogFacility::_LogPrefix, logmsg.str(), level, false);
}


/**  Alternate LogMessage allowing control of the newline character. This
 *   mainly applies to not syslog logstreams.
 */
void
LogFacility::LogMessage ( const std::string & entry, int level, bool newline )
{
    return LogFacility::LogMessage(LogFacility::_LogName, entry, level, newline);
}


void
LogFacility::LogMessage ( const std::string & logname, 
                          const std::string & entry, 
                          int   level,
                          bool  newline )
{
#   ifndef WIN32
    if ( LogFacility::_Syslog && LogFacility::Lock() ) {
        ::syslog(level, "%s:%s", LogFacility::_LogPrefix.c_str(), entry.c_str());
        LogFacility::Unlock();
    }
#   endif

    if ( level == LOGFAC_DEBUG && ! LogFacility::GetDebug() )
        return;
               
    if ( logname.empty() || LogFacility::_Broadcast )
        LogFacility::LogToAllStreams(entry, newline);
    else
        LogFacility::LogToStream(logname, entry, newline);

    return;
}

// ----------------------------------------------------------------------

/**  Broadcasts a log message to all enabled log streams */
void
LogFacility::LogToAllStreams ( const std::string & entry, bool newline )
{
    std::list<std::string>  dead;

    StreamMap::iterator              sIter;
    std::list<std::string>::iterator dIter;
  
    if ( ! LogFacility::Lock() )
        return;

    for ( sIter = _StreamMap.begin(); sIter != _StreamMap.end(); ++sIter ) {
        std::ostream * strm = sIter->second.logStream;

        if ( strm == NULL ) {
            dead.push_back(sIter->first);
            continue;
        }

        if ( ! sIter->second.enabled )
            continue;

        if ( ! sIter->second.logPrefix.empty() )
            *(strm) << sIter->second.logPrefix << ": ";
        else if ( ! LogFacility::_LogPrefix.empty() )
            *(strm) << LogFacility::_LogPrefix << ": ";

        if ( LogFacility::_LogTime > 0 && sIter->second.showTime )
            *(strm) << LogFacility::_LogTimeStr << " : ";

        *(strm) << entry;

        if ( newline )
            *(strm) << std::endl;

        strm->flush();
    }

    // clear out dead streams
    for ( dIter = dead.begin(); dIter != dead.end(); ++dIter )
        _StreamMap.erase(*dIter);
    dead.clear();

    LogFacility::Unlock();

    return;
}

// ----------------------------------------------------------------------


void
LogFacility::LogToStream ( const std::string & logname, 
                           const std::string & entry,
                           bool  newline )
{
    if ( ! LogFacility::Lock() )
        return;

    StreamMap::iterator  sIter = LogFacility::_StreamMap.find(logname);

    if ( sIter != LogFacility::_StreamMap.end() ) {
        std::ostream * strm = sIter->second.logStream;

        if ( ! sIter->second.logPrefix.empty() )
            *(strm) <<  sIter->second.logPrefix << ": ";
        else if ( ! LogFacility::_LogPrefix.empty() )
            *(strm) << LogFacility::_LogPrefix  << ": ";

        if ( LogFacility::_LogTime > 0 && sIter->second.showTime )
            *(strm) << LogFacility::_LogTimeStr << " : ";

        *(strm) << entry;

        if ( newline )
            *(strm) << std::endl;

        strm->flush();
    }

    LogFacility::Unlock();

    return;
}

// ----------------------------------------------------------------------

void
LogFacility::CloseLogFacility()
{
    LogFacility::CloseSyslog();
    LogFacility::RemoveLogStreams(false);
}


void
LogFacility::CloseSyslog()
{
#   ifndef WIN32
    if ( LogFacility::_Syslog ) {
        ::syslog(LOGFAC_NOTICE, "==== LogFacility Closing ====");
        ::closelog();
        LogFacility::_Syslog = false;
    }
#   endif

    return;
}


std::ostream*
LogFacility::CloseLogFile ( const std::string & logname, bool del )
{
    std::ofstream  * fstrm = NULL;

    if ( LogFacility::IsOpen(logname) )
        LogFacility::LogMessage(logname, "==== LogFacility Closing ====");

    fstrm = (std::ofstream*) LogFacility::RemoveLogStream(logname);

    if ( fstrm != NULL ) {
        fstrm->close();
        if ( del ) {
            delete fstrm;
            fstrm = NULL;
        }
    }

    return fstrm;
}

// ----------------------------------------------------------------------

void
LogFacility::SetDefaultLogPrefix ( const std::string & prefix )
{
    LogFacility::_LogPrefix = prefix;
}

void
LogFacility::SetLogPrefix ( const std::string & logname, const std::string & prefix )
{
    if ( ! LogFacility::Lock() )
        return;

    StreamMap::iterator  sIter = _StreamMap.find(logname);
    if ( sIter != _StreamMap.end() )
        sIter->second.logPrefix = prefix;

    LogFacility::Unlock();

    return;
}

std::string
LogFacility::GetLogPrefix ( const std::string & logName )
{
    std::string  prefix;

    if ( logName.empty() )
        return LogFacility::_LogPrefix;

    if ( ! LogFacility::Lock() )
        return prefix;

    StreamMap::iterator  sIter = _StreamMap.find(logName);
    if ( sIter != _StreamMap.end() )
        prefix = sIter->second.logPrefix;

    LogFacility::Unlock();

    return prefix;
}

bool
LogFacility::SetDefaultLogName ( const std::string & logName )
{
    if ( logName.empty() )
        return false;

    LogFacility::_LogName = logName;

    return true;
}

std::string
LogFacility::GetDefaultLogName()
{
    return LogFacility::_LogName;
}

// ----------------------------------------------------------------------

/**  RotateLogFile will automatically rotate a file log each day based 
  *  on the 'localtime' result of the provided time. Each logstream tracks
  *  the day of year value, so if this function is called within the same 
  *  day, the log will not be rotated (and the function returns false).
  *  When called with a time that is of a different day of year, the existing
  *  log stream is closed and a new logstream is created based on the logname
  *  with an appended '.log_YYYYMMDD'.
  *  It is up to the user to ensure that the underlying logstream is a proper
  *  log file stream.
 **/
bool
LogFacility::RotateLogFile ( const std::string & logName, const time_t & now )
{
    if ( logName.empty() )
        return false;

    StreamMap::iterator sIter = _StreamMap.find(logName);
    if ( sIter == _StreamMap.end() )
        return false;

    char       datestr[64];
    struct tm *ltm     = ::localtime(&now);
    int        today   = ltm->tm_yday;
    bool       rotated = false;

    if ( sIter->second.today == 0 ) 
    {
        sIter->second.today = today;
        return rotated;
    }

    if ( sIter->second.today != today ) 
    {
        LogFacility::CloseLogFile(logName);

        ::strftime(datestr, 64, ".log_%Y%02m%02d", ltm);
        std::string logfile = logName;
        logfile.append(datestr);
        sIter->second.today = today;

        rotated = LogFacility::OpenLogFile(logName, LogFacility::GetLogPrefix(), logfile, true);
    }

    return rotated;
}

// ----------------------------------------------------------------------

bool
LogFacility::ShowLogTime ( const std::string & logname, bool showTime )
{
    if ( ! LogFacility::Lock() )
        return false;

    StreamMap::iterator sIter = _StreamMap.find(logname);

    if ( sIter != _StreamMap.end() )
        sIter->second.showTime = showTime;

    LogFacility::Unlock();

    return true;
}

void
LogFacility::SetLogTime ( const time_t & now )
{
    if ( LogFacility::Lock() ) {
        LogFacility::_LogTime  = now;
    
        if ( LogFacility::_LogTime > 0 )
            LogFacility::_LogTimeStr = LogFacility::GetTimeString(now);
        LogFacility::Unlock();
    }

    return;
}

time_t
LogFacility::GetLogTime()
{
    time_t  logtime = 0;

    if ( LogFacility::Lock() ) {
        logtime = LogFacility::_LogTime;
        LogFacility::Unlock();
    }

    return logtime;
}

// ----------------------------------------------------------------------

std::string
LogFacility::GetTimeString ( const time_t & now )
{
    char         tstr[32];
    std::string  timestr;

#   ifdef WIN32
    if ( ::ctime_s(tstr, 32, &now) != 0 )
        return timestr;
#   else
    ::ctime_r(&now, tstr);
#   endif

    timestr = tstr;
    timestr = timestr.substr(0, timestr.length() - 1);

    return timestr;
}

// ----------------------------------------------------------------------

int
LogFacility::GetDayOfYear ( const time_t & now )
{
    struct tm *ltm = ::localtime(&now);
    int      today = 0;
    
    if ( ltm ) 
        today = ltm->tm_yday;

    return today;
}

// ----------------------------------------------------------------------

void
LogFacility::InitLogMessage ( const std::string & logname )
{
    Message      logMsg;
    std::string  tstr;
    time_t       now;

    now  = ::time(NULL);
    tstr = LogFacility::GetTimeString(now);

    logMsg << "Log Initialized at '" << tstr << "'";

    LogFacility::LogMessage(logname, logMsg.str());
}

// ----------------------------------------------------------------------

bool
LogFacility::Lock()
{ 
#   ifndef WIN32
#   ifdef PTHREADS
    if ( LogFacility::_InitLock && LogFacility::_TryLock ) {
        if ( LogFacility::_Lock.tryLock() <= 0 )
            return false;
    } else if ( LogFacility::_InitLock ) {
        if ( LogFacility::_Lock.lock() <= 0 )
            return false;
    }
#   endif
#   endif

    return true;
}

void
LogFacility::Unlock()
{
#   ifndef WIN32
#   ifdef PTHREADS
    if ( LogFacility::_InitLock )
        LogFacility::_Lock.unlock();
#   endif
#   endif 
    return;
}

// ----------------------------------------------------------------------


} // namespace


//  _TCANETPP_LOGFACILITY_CPP_

