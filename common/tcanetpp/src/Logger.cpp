/**
  *  Logger
  * 
  *  @Author  tcarland@gmail.com
 **/
#define _TCANETPP_LOGGER_CPP_

#include <list>
#include <memory>
#include <iostream>
#include <fstream>

#include "Logger.h"


namespace tcanetpp {


// ----------------------------------------------------------------------
//  Initialization

Logger::StreamMap  Logger::_StreamMap  = StreamMap();

#ifndef WIN32
tcanetpp::ThreadLock  Logger::_Lock    = tcanetpp::ThreadLock();
#endif


time_t             Logger::_LogTime    = 0;
bool               Logger::_Init       = false;
bool               Logger::_InitLock   = false;
bool               Logger::_TryLock    = false;
bool               Logger::_Syslog     = false;
bool               Logger::_Enabled    = false;
bool               Logger::_Debug      = false;
std::ostream*      Logger::_LogStream  = NULL;
std::string        Logger::_FileName   = "";
std::string        Logger::_LogPrefix  = "tcanetpp::Logger";
std::string        Logger::_LogTimeStr = "";


// ----------------------------------------------------------------------

/**  Initializes a thread-safe version of the Logger. */
bool
Logger::InitThreaded ( bool trylock )
{
#   ifdef WIN32
    return false;
#   else
    Logger::_TryLock  = trylock;
    Logger::_InitLock = true;
#   endif
    return true;
}

// ----------------------------------------------------------------------

bool
Logger::OpenLogFile ( const std::string & prefix, 
                      const std::string & filename, bool append )
{
    std::ofstream       * fstrm = NULL;
    StreamMap::iterator   sIter;

    sIter = Logger::_StreamMap.find(Logger::_FileName);
    
    if ( sIter == Logger::_StreamMap.end() ) {
        fstrm  = new std::ofstream;

        if ( append )
            fstrm->open(filename.c_str(), std::ios::out | std::ios::app);
        else
            fstrm->open(filename.c_str(), std::ios::out | std::ios::trunc);

    } else {
        Logger::CloseFilelog();
        std::auto_ptr<std::ofstream>  newfstrm(new std::ofstream());

        if ( append )
            newfstrm->open( filename.c_str(), std::ios::out | std::ios::app );
        else
            newfstrm->open( filename.c_str(), std::ios::out | std::ios::trunc );

        if ( ! newfstrm->is_open() )
            return false;

        fstrm  = newfstrm.release();
    }

    Logger::_LogPrefix = prefix;
    Logger::_LogStream = (std::ostream*) fstrm;
    Logger::_FileName  = filename;
    Logger::_Enabled   = true;
    
    Logger::AddStream(Logger::_LogPrefix, Logger::_LogStream);
    Logger::InitLogMessage();

    return fstrm->is_open();
}

// ----------------------------------------------------------------------

bool
Logger::OpenSyslog ( const std::string & prefix, int facility )
{
#   ifndef WIN32
    if ( Logger::_Syslog )
        Logger::CloseSyslog();

    ::openlog(prefix.c_str(), LOG_PID, facility);
    Logger::_Syslog   = true;
    Logger::_Enabled  = true;

    Logger::InitLogMessage();

    return true;
#endif
    return false;
}

// ----------------------------------------------------------------------

bool
Logger::OpenLogStream ( const std::string & prefix, std::ostream * stream )
{
     bool r = Logger::AddStream(prefix, stream);
     
     if ( r )
         Logger::_Enabled = true;
     
     return r;
}

// ----------------------------------------------------------------------

bool
Logger::AddStream ( const std::string & name, std::ostream * stream )
{
    StreamMap::iterator  sIter;

    if ( stream == NULL )
        return false;

    sIter = Logger::_StreamMap.find(name);

    if ( sIter != Logger::_StreamMap.end() )
        return false;

    Logger::_StreamMap[name] = stream;

    return true;
}

std::ostream*
Logger::RemoveStream ( const std::string & name )
{
    std::ostream        *ptr = NULL;
    StreamMap::iterator  sIter;

    sIter = Logger::_StreamMap.find(name);

    if ( sIter == Logger::_StreamMap.end() )
        return ptr;

    if ( sIter->second != NULL )
        ptr = sIter->second;

    Logger::_StreamMap.erase(sIter);

    return ptr;
}

// ----------------------------------------------------------------------

std::string
Logger::GetFileName()
{
    return Logger::_FileName;
}


void
Logger::SetEnabled( bool enabled )
{
    Logger::_Enabled = enabled;
}

bool
Logger::GetEnabled()
{
    return Logger::_Enabled;
}


void
Logger::SetDebug ( bool d )
{
    Logger::_Debug = d;
}

bool
Logger::GetDebug()
{
    return Logger::_Debug;
}

bool
Logger::IsOpen()
{
    std::ofstream *  fstrm = NULL;
  
    if ( Logger::_LogStream != NULL ) {
        fstrm = (std::ofstream*) Logger::_LogStream;
        return fstrm->is_open();
    }

    if ( Logger::_StreamMap.size() == 0 )
        return false;

    return true;
}

// ----------------------------------------------------------------------

void
Logger::LogMessage ( const std::string & entry, int level )
{
    return Logger::LogMessage(Logger::_LogPrefix, entry, level);
}


void
Logger::LogMessage ( const std::string & prefix, 
                     const std::string & entry, 
                     int level )
{
    if ( ! Logger::_Enabled )
        return;

#   ifndef WIN32
    if ( Logger::_Syslog && Logger::Lock() ) {
        ::syslog(level, "%s:%s", prefix.c_str(), entry.c_str());
        Logger::_Lock.unlock();
    }
#   endif

    Logger::LogToStream(Logger::_FileName, prefix, entry);

    return;
}

// ----------------------------------------------------------------------

void
Logger::LogToAllStreams ( const std::string & prefix, const std::string & entry )
{
    StreamMap::iterator     sIter; 
    std::list<std::string>  dead;
    std::list<std::string>::iterator dIter;
  
    if ( ! Logger::Lock() )
        return;

    // log to all secondary streams
    for ( sIter = _StreamMap.begin(); sIter != _StreamMap.end(); ++sIter ) {
        if ( sIter->second == NULL ) {
            dead.push_back(sIter->first);
            continue;
        }
        *(sIter->second) << prefix << ": ";
        if ( Logger::_LogTime > 0 )
            *(sIter->second) << Logger::_LogTimeStr << " : ";
        *(sIter->second) << entry << std::endl;
        sIter->second->flush();
    }

    // clear out dead streams
    for ( dIter = dead.begin(); dIter != dead.end(); ++dIter )
        _StreamMap.erase(*dIter);
    dead.clear();

    Logger::Unlock();

    return;
}

// ----------------------------------------------------------------------

void
Logger::LogToStream ( const std::string & streamName, 
                      const std::string & entry )
{
    return Logger::LogToStream(streamName, streamName, entry);
}


void
Logger::LogToStream ( const std::string & prefix, 
                      const std::string & streamName, 
                      const std::string & entry )
{
    StreamMap::iterator  sIter;

    if ( ! Logger::Lock() )
        return;

    sIter = Logger::_StreamMap.find(streamName);
    if ( sIter == Logger::_StreamMap.end() )
        return;

    *(sIter->second) << prefix << ": ";
    if ( Logger::_LogTime > 0 )
        *(sIter->second) << Logger::_LogTimeStr << " : ";
    *(sIter->second) << entry << std::endl;
    sIter->second->flush();

    Logger::Unlock();

    return;
}

// ----------------------------------------------------------------------

void
Logger::CloseLogger()
{
    Logger::CloseSyslog();
    Logger::CloseFilelog();

    Logger::_Enabled = false;
    Logger::_Syslog  = false;

    Logger::_StreamMap.clear();

    return;
}


void
Logger::CloseSyslog()
{
#   ifndef WIN32
    if ( Logger::_Syslog ) {
        ::syslog(LOGGER_NOTICE, "==== Logger Closing ====");
        ::closelog();
        Logger::_Syslog = false;
    }
#   endif

    if ( Logger::_LogStream == NULL )
        Logger::_Enabled = false;

    return;
}


void
Logger::CloseFilelog()
{
    std::ofstream  * fstrm = NULL;

    Logger::LogMessage("==== Logger Closing ====");

    fstrm = (std::ofstream*) Logger::RemoveStream(Logger::_FileName);

    if ( fstrm != NULL && fstrm == Logger::_LogStream ) {
        fstrm->close();
        delete Logger::_LogStream;
        Logger::_LogStream = NULL;
        Logger::_FileName  = "";
    }

    return;
}

// ----------------------------------------------------------------------

void
Logger::SetLogPrefix ( const std::string & prefix )
{
    if ( ! prefix.empty() )
        Logger::_LogPrefix = prefix;
    return;
}

std::string
Logger::GetLogPrefix()
{
    return Logger::_LogPrefix;
}

// ----------------------------------------------------------------------

void
Logger::SetLogTime ( const time_t & now )
{
    if ( Logger::Lock() ) {
        Logger::_LogTime  = now;
    
        if ( Logger::_LogTime > 0 )
            Logger::_LogTimeStr = Logger::GetTimeString(now);
        Logger::Unlock();
    }

    return;
}

// ----------------------------------------------------------------------

std::string
Logger::GetTimeString ( const time_t & now )
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
Logger::InitLogMessage()
{
    LoggerMsg    logMsg;
    std::string  tstr;
    time_t       now;

    now  = ::time(NULL);
    tstr = Logger::GetTimeString(now);

    logMsg << "Log Initialized at '" << tstr << "'";

    Logger::LogMessage(logMsg.str());
}

// ----------------------------------------------------------------------

bool
Logger::Lock()
{ 
#   ifndef WIN32
    if ( Logger::_InitLock && Logger::_TryLock ) {
        if ( Logger::_Lock.tryLock() <= 0 )
            return false;
    } else if ( Logger::_InitLock ) {
        if ( Logger::_Lock.lock() <= 0 )
            return false;
    }
#   endif

    return true;
}

void
Logger::Unlock()
{
#   ifndef WIN32
    if ( Logger::_InitLock )
        Logger::_Lock.unlock();
#   endif 
    return;
}

// ----------------------------------------------------------------------


} // namespace


//  _TCANETPP_LOGGER_CPP_
