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
std::ostream*      LogFacility::_LogStream  = NULL;
std::string        LogFacility::_FileName   = "";
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
LogFacility::OpenLogFile ( const std::string & prefix, const std::string & filename, bool append )
{
    std::ofstream       * fstrm = NULL;
    StreamMap::iterator   sIter;

    sIter = LogFacility::_StreamMap.find(LogFacility::_FileName);
    
    if ( sIter == LogFacility::_StreamMap.end() ) {
        fstrm  = new std::ofstream;

        if ( append )
            fstrm->open(filename.c_str(), std::ios::out | std::ios::app);
        else
            fstrm->open(filename.c_str(), std::ios::out | std::ios::trunc);

    } else {
        LogFacility::CloseLogFile();
        std::auto_ptr<std::ofstream>  newfstrm(new std::ofstream());

        if ( append )
            newfstrm->open( filename.c_str(), std::ios::out | std::ios::app );
        else
            newfstrm->open( filename.c_str(), std::ios::out | std::ios::trunc );

        if ( ! newfstrm->is_open() )
            return false;

        fstrm  = newfstrm.release();
    }

    LogFacility::_LogPrefix = prefix;
    LogFacility::_LogStream = (std::ostream*) fstrm;
    LogFacility::_FileName  = filename;
    LogFacility::_Enabled   = true;
    
    LogFacility::AddLogStream(LogFacility::_LogPrefix, LogFacility::_LogStream);
    LogFacility::InitLogMessage();

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
LogFacility::OpenLogStream ( const std::string & prefix, std::ostream * stream )
{
     bool r = LogFacility::AddLogStream(prefix, stream);
     
     if ( r )
         LogFacility::_Enabled = true;

     return r;
}

// ----------------------------------------------------------------------

bool
LogFacility::AddLogStream ( const std::string & name, std::ostream * stream )
{
    StreamMap::iterator  sIter;

    if ( stream == NULL )
        return false;

    sIter = LogFacility::_StreamMap.find(name);

    if ( sIter != LogFacility::_StreamMap.end() )
        return false;

    LogFacility::_StreamMap[name] = stream;

    return true;
}

std::ostream*
LogFacility::RemoveStream ( const std::string & name )
{
    std::ostream        *ptr = NULL;
    StreamMap::iterator  sIter;

    sIter = LogFacility::_StreamMap.find(name);

    if ( sIter == LogFacility::_StreamMap.end() )
        return ptr;

    if ( sIter->second != NULL )
        ptr = sIter->second;

    LogFacility::_StreamMap.erase(sIter);

    return ptr;
}

// ----------------------------------------------------------------------

std::string
LogFacility::GetFileName()
{
    return LogFacility::_FileName;
}


void
LogFacility::SetEnabled( bool enabled )
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
    std::ofstream *  fstrm = NULL;
  
    if ( LogFacility::_LogStream != NULL ) {
        fstrm = (std::ofstream*) LogFacility::_LogStream;
        return fstrm->is_open();
    }

    if ( LogFacility::_StreamMap.size() == 0 )
        return false;

    return true;
}

// ----------------------------------------------------------------------

void
LogFacility::LogMessage ( const std::string & entry, int level )
{
    return LogFacility::LogMessage(LogFacility::_LogPrefix, entry, level);
}


void
LogFacility::LogMessage ( const std::string & prefix, 
                          const std::string & entry, 
                          int level )
{
    if ( ! LogFacility::_Enabled )
        return;

#   ifndef WIN32
    if ( LogFacility::_Syslog && LogFacility::Lock() ) {
        ::syslog(level, "%s:%s", prefix.c_str(), entry.c_str());
        LogFacility::_Lock.unlock();
    }
#   endif
               
    if ( LogFacility::_FileName.empty() || LogFacility::_Broadcast )
        LogFacility::LogToAllStreams(prefix, entry);
    else
        LogFacility::LogToStream(LogFacility::_FileName, prefix, entry);

    return;
}

// ----------------------------------------------------------------------

void
LogFacility::LogToAllStreams ( const std::string & prefix, const std::string & entry )
{
    StreamMap::iterator     sIter; 
    std::list<std::string>  dead;
    std::list<std::string>::iterator dIter;
  
    if ( ! LogFacility::Lock() )
        return;

    // log to all secondary streams
    for ( sIter = _StreamMap.begin(); sIter != _StreamMap.end(); ++sIter ) {
        if ( sIter->second == NULL ) {
            dead.push_back(sIter->first);
            continue;
        }
        *(sIter->second) << prefix << ": ";
        if ( LogFacility::_LogTime > 0 )
            *(sIter->second) << LogFacility::_LogTimeStr << " : ";
        *(sIter->second) << entry << std::endl;
        sIter->second->flush();
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
LogFacility::LogToStream ( const std::string & streamName, 
                      const std::string & entry )
{
    return LogFacility::LogToStream(streamName, streamName, entry);
}


void
LogFacility::LogToStream ( const std::string & prefix, 
                      const std::string & streamName, 
                      const std::string & entry )
{
    StreamMap::iterator  sIter;

    if ( ! LogFacility::Lock() )
        return;

    sIter = LogFacility::_StreamMap.find(streamName);
    if ( sIter == LogFacility::_StreamMap.end() )
        return;

    *(sIter->second) << prefix << ": ";
    if ( LogFacility::_LogTime > 0 )
        *(sIter->second) << LogFacility::_LogTimeStr << " : ";
    *(sIter->second) << entry << std::endl;
    sIter->second->flush();

    LogFacility::Unlock();

    return;
}

// ----------------------------------------------------------------------

void
LogFacility::CloseLogFacility()
{
    LogFacility::CloseSyslog();
    LogFacility::CloseLogFile();

    LogFacility::_Enabled = false;
    LogFacility::_Syslog  = false;

    LogFacility::_StreamMap.clear();

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

    if ( LogFacility::_LogStream == NULL )
        LogFacility::_Enabled = false;

    return;
}


void
LogFacility::CloseLogFile()
{
    std::ofstream  * fstrm = NULL;

    LogFacility::LogMessage("==== LogFacility Closing ====");

    fstrm = (std::ofstream*) LogFacility::RemoveStream(LogFacility::_FileName);

    if ( fstrm != NULL && fstrm == LogFacility::_LogStream ) {
        fstrm->close();
        delete LogFacility::_LogStream;
        LogFacility::_LogStream = NULL;
        LogFacility::_FileName  = "";
    }

    return;
}

// ----------------------------------------------------------------------

void
LogFacility::SetLogPrefix ( const std::string & prefix )
{
    if ( ! prefix.empty() )
        LogFacility::_LogPrefix = prefix;
    return;
}

std::string
LogFacility::GetLogPrefix()
{
    return LogFacility::_LogPrefix;
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
LogFacility::InitLogMessage()
{
    Message      logMsg;
    std::string  tstr;
    time_t       now;

    now  = ::time(NULL);
    tstr = LogFacility::GetTimeString(now);

    logMsg << "Log Initialized at '" << tstr << "'";

    LogFacility::LogMessage(logMsg.str());
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

