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

#include "LogFacility.h"


namespace tcanetpp {


// ----------------------------------------------------------------------
//  Initialization

LogFacility::StreamMap  LogFacility::_StreamMap  = StreamMap();

#ifndef WIN32
tcanetpp::ThreadLock  LogFacility::_Lock    = tcanetpp::ThreadLock();
#endif


time_t             LogFacility::_LogTime    = 0;
bool               LogFacility::_Init       = false;
bool               LogFacility::_InitLock   = false;
bool               LogFacility::_TryLock    = false;
bool               LogFacility::_Syslog     = false;
bool               LogFacility::_Enabled    = false;
bool               LogFacility::_Broadcast  = false;
bool               LogFacility::_Debug      = false;
std::string        LogFacility::_LogName   = "";
std::string        LogFacility::_LogPrefix  = "tcanetpp::LogFacility";
std::string        LogFacility::_LogTimeStr = "";


// ----------------------------------------------------------------------

/**  Initializes a thread-safe version of the LogFacility. */
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
    
    if ( sIter == LogFacility::_StreamMap.end() )
        return false;

    std::auto_ptr<std::ofstream>  newfstrm(new std::ofstream());

    if ( append )
        newfstrm->open( filename.c_str(), std::ios::out | std::ios::app );
    else
        newfstrm->open( filename.c_str(), std::ios::out | std::ios::trunc );

    if ( ! newfstrm->is_open() )
        return false;

    fstrm  = newfstrm.release();

    LogFacility::_Enabled  = true;
    
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
LogFacility::OpenLogStream ( const std::string & name, const std::string & prefix, std::ostream * stream )
{
     bool r = LogFacility::AddLogStream(name, prefix, stream);
     
     if ( r )
         LogFacility::_Enabled = true;

     return r;
}

// ----------------------------------------------------------------------

bool
LogFacility::AddLogStream ( const std::string & name, const std::string & prefix, std::ostream * stream )
{
    bool result = false;
    if ( stream == NULL ||  ! LogFacility::Lock() )
        return result;

    StreamMap::iterator  sIter = LogFacility::_StreamMap.find(name);
    if ( sIter == LogFacility::_StreamMap.end() ) {
        LogStream  lstrm(name, prefix, stream);
        LogFacility::_StreamMap[name] = lstrm;
        result = true;
    }

    LogFacility::Unlock();

    return result;
}

std::ostream*
LogFacility::RemoveLogStream ( const std::string & name )
{
    std::ostream        *ptr = NULL;
    StreamMap::iterator  sIter;

    if ( ! LogFacility::Lock() )
        return ptr;

    sIter = LogFacility::_StreamMap.find(name);
    if ( sIter != LogFacility::_StreamMap.end() ) {
        ptr = sIter->second.logStream;
        LogFacility::_StreamMap.erase(sIter);
    }

    LogFacility::Unlock();

    return ptr;
}

void
LogFacility::RemoveLogStreams ( bool del )
{
    StreamMap::iterator     sIter;

    if ( ! LogFacility::Lock() )
        return;

    for ( sIter = _StreamMap.begin(); sIter != _StreamMap.end(); ++sIter ) {
        if ( sIter->second.logStream == NULL )
            continue;
        
        if ( del )
            delete sIter->second.logStream;
    }

    _StreamMap.clear();
    LogFacility::Unlock();

    return;
}


// ----------------------------------------------------------------------

void
LogFacility::SetEnabled ( bool enabled )
{
    LogFacility::_Enabled = enabled;
}

bool
LogFacility::GetEnabled()
{
    return LogFacility::_Enabled;
}

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
    if ( LogFacility::_StreamMap.size() == 0 )
        return false;

    return LogFacility::_Enabled;
}

// ----------------------------------------------------------------------

void
LogFacility::LogMessage ( LogFacility::Message & logmsg, int level )
{
    return LogFacility::LogMessage(LogFacility::_LogPrefix, logmsg.str(), level, false);
}


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
    if ( ! LogFacility::_Enabled )
        return;

#   ifndef WIN32
    if ( LogFacility::_Syslog && LogFacility::Lock() ) {
        ::syslog(level, "%s:%s", LogFacility::_LogPrefix.c_str(), entry.c_str());
        LogFacility::Unlock();
    }
#   endif
               
    if ( logname.empty() || LogFacility::_Broadcast )
        LogFacility::LogToAllStreams(entry, newline);
    else
        LogFacility::LogToStream(logname, entry, newline);

    return;
}

// ----------------------------------------------------------------------

void
LogFacility::LogToAllStreams ( const std::string & entry, bool newline )
{
    std::list<std::string>  dead;

    StreamMap::iterator              sIter;
    std::list<std::string>::iterator dIter;
  
    if ( ! LogFacility::Lock() )
        return;

    // log to all secondary streams
    for ( sIter = _StreamMap.begin(); sIter != _StreamMap.end(); ++sIter ) {
        std::ostream * strm = sIter->second.logStream;

        if ( strm == NULL ) {
            dead.push_back(sIter->first);
            continue;
        }

        std::string & prefix = LogFacility::_LogPrefix;

        if ( ! sIter->second.logPrefix.empty() )
            prefix = sIter->second.logPrefix;

        if ( ! prefix.empty() )
            *(strm) << prefix << ": ";

        if ( LogFacility::_LogTime > 0 )
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
        std::string  prefix = LogFacility::_LogPrefix;

        if ( ! sIter->second.logPrefix.empty() )
            prefix = sIter->second.logPrefix;

        if ( ! prefix.empty() )
            *(strm) << prefix << ": ";

        if ( LogFacility::_LogTime > 0 )
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
    LogFacility::RemoveLogStreams(true);

    LogFacility::_Enabled = false;
    LogFacility::_Syslog  = false;

    return;
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

    if ( LogFacility::_StreamMap.empty() )
        LogFacility::_Enabled = false;

    return;
}


std::ostream*
LogFacility::CloseLogFile ( const std::string & logname, bool del )
{
    std::ofstream  * fstrm = NULL;

    LogFacility::LogMessage("==== LogFacility Closing ====");

    fstrm = (std::ofstream*) LogFacility::RemoveLogStream(logname);

    if ( fstrm != NULL ) {
        fstrm->close();
        if ( del ) {
            delete fstrm;
            fstrm = NULL;
        }
    }

    if ( LogFacility::_StreamMap.empty() )
        LogFacility::_Enabled = false;

    return fstrm;
}

// ----------------------------------------------------------------------

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

void
LogFacility::SetDefaultLogPrefix ( const std::string & prefix )
{
    LogFacility::_LogPrefix = prefix;
}

std::string
LogFacility::GetLogPrefix ( const std::string & logname )
{
    std::string  prefix;

    if ( logname.empty() )
        return LogFacility::_LogPrefix;

    if ( ! LogFacility::Lock() )
        return prefix;

    StreamMap::iterator  sIter = _StreamMap.find(logname);
    if ( sIter != _StreamMap.end() )
        prefix = sIter->second.logPrefix;

    LogFacility::Unlock();

    return prefix;
}

// ----------------------------------------------------------------------

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
    char * tptr = ::ctime(&now);
    ::strncpy(tstr, tptr, 32);
#   else
    ::ctime_r(&now, tstr);
#   endif
    timestr = tstr;
    timestr = timestr.substr(0, timestr.length() - 1);

    return timestr;
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
    if ( LogFacility::_InitLock && LogFacility::_TryLock ) {
        if ( LogFacility::_Lock.tryLock() <= 0 )
            return false;
    } else if ( LogFacility::_InitLock ) {
        if ( LogFacility::_Lock.lock() <= 0 )
            return false;
    }
#   endif

    return true;
}

void
LogFacility::Unlock()
{
#   ifndef WIN32
    if ( LogFacility::_InitLock )
        LogFacility::_Lock.unlock();
#   endif 
    return;
}

// ----------------------------------------------------------------------


} // namespace


//  _TCANETPP_LOGFACILITY_CPP_

