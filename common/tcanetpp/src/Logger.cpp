/**
  *  Logger
  * 
  *  @Author  tcarland@gmail.com
 **/
#define _TCANETPP_LOGGER_CPP_

#include <memory>
#include <iostream>
#include <fstream>
#include <list>

#include "Logger.h"

namespace tcanetpp {


/*  Static initialization  */
Logger::StreamMap     Logger::strmMap = StreamMap();

#ifndef WIN32
tcanetpp::ThreadLock  Logger::lock    = tcanetpp::ThreadLock();
#endif


time_t             Logger::logTime    = 0;
bool               Logger::init       = false;
bool               Logger::initlock   = false;
bool               Logger::trylock    = false;
bool               Logger::syslog     = false;
bool               Logger::enabled    = false;
bool               Logger::debug      = false;
std::ostream*      Logger::logStream  = NULL;
std::string        Logger::fileName   = "";
std::string        Logger::logPrefix  = "tcanetpp::Logger";
std::string        Logger::logTimeStr = "";


// ----------------------------------------------------------------------

/**  Initializes a thread-safe version of the Logger. */
bool
Logger::InitThreaded ( bool trylock )
{
#   ifdef WIN32
    return false;
#   else
    Logger::trylock  = trylock;
    Logger::initlock = true;
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

    sIter = Logger::strmMap.find(Logger::fileName);
    
    if ( sIter == Logger::strmMap.end() ) {
        fstrm  = new std::ofstream;

        if ( append )
            fstrm->open( filename.c_str(), std::ios::out | std::ios::app );
        else
            fstrm->open( filename.c_str(), std::ios::out | std::ios::trunc );

    } else {
        Logger::CloseFilelog();

        std::auto_ptr< std::ofstream >  newfstrm( new std::ofstream() );

        if ( append )
            newfstrm->open( filename.c_str(), std::ios::out | std::ios::app );
        else
            newfstrm->open( filename.c_str(), std::ios::out | std::ios::trunc );

        if ( ! newfstrm->is_open() )
            return false;

        fstrm  = newfstrm.release();
    }

    Logger::fileName  = filename;
    Logger::logPrefix = prefix;
    Logger::logStream = (std::ostream*) fstrm;
    Logger::enabled   = true;
    
    Logger::AddStream(Logger::logPrefix, Logger::logStream);
    Logger::InitLogMsg();

    return fstrm->is_open();
}

// ----------------------------------------------------------------------

bool
Logger::OpenSyslog ( const std::string & prefix, int facility )
{
#   ifndef WIN32
    if ( Logger::syslog )
        Logger::CloseSyslog();

    ::openlog(logPrefix.c_str(), LOG_PID, facility);
    syslog   = true;
    enabled  = true;

    Logger::InitLogMsg();

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
         Logger::enabled = true;
     return r;
}

// ----------------------------------------------------------------------

bool
Logger::AddStream ( const std::string & name, std::ostream * stream )
{
    StreamMap::iterator  sIter;

    if ( stream == NULL )
        return false;

    sIter = strmMap.find(name);

    if ( sIter != strmMap.end() )
        return false;

    strmMap[name] = stream;

    return true;
}

std::ostream*
Logger::RemoveStream ( const std::string & name )
{
    std::ostream        *ptr = NULL;
    StreamMap::iterator  sIter;

    sIter = strmMap.find(name);

    if ( sIter == strmMap.end() )
        return ptr;

    if ( sIter->second != NULL )
        ptr = sIter->second;

    strmMap.erase(sIter);

    return ptr;
}

// ----------------------------------------------------------------------

std::string
Logger::GetFileName()
{
    return fileName;
}


void
Logger::SetEnabled( bool enabled )
{
    enabled = enabled;
}

bool
Logger::GetEnabled()
{
    return enabled;
}


void
Logger::SetDebug ( bool d )
{
    debug = d;
}

bool
Logger::Debug()
{
    return debug;
}

bool
Logger::IsOpen()
{
    std::ofstream *  fstrm = NULL;
  
    if ( logStream != NULL ) {
        fstrm = (std::ofstream*) logStream;
        return fstrm->is_open();
    }

    if ( strmMap.size() == 0 )
        return false;

    return true;
}

// ----------------------------------------------------------------------

void
Logger::LogMessage ( const std::string & entry, int level )
{
    return Logger::LogMessage(logPrefix, entry, level);
}


void
Logger::LogMessage ( const std::string & prefix, 
                     const std::string & entry, 
                     int level )
{
    if ( ! enabled )
        return;

#   ifndef WIN32
    if ( syslog && Logger::Lock() ) {
        ::syslog(level, "%s:%s", prefix.c_str(), entry.c_str());
        Logger::lock.unlock();
    }
#   endif

    Logger::LogToStream(Logger::fileName, prefix, entry);

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
    for ( sIter = strmMap.begin(); sIter != strmMap.end(); ++sIter ) {
        if ( sIter->second == NULL ) {
            dead.push_back(sIter->first);
            continue;
        }
        *(sIter->second) << prefix << ": ";
        if ( Logger::logTime > 0 )
            *(sIter->second) << Logger::logTimeStr << " : ";
        *(sIter->second) << entry << std::endl;
        sIter->second->flush();
    }

    // clear out dead streams
    for ( dIter = dead.begin(); dIter != dead.end(); ++dIter )
        strmMap.erase(*dIter);
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
Logger::LogToStream ( const std::string & streamName, 
                      const std::string & prefix,
                      const std::string & entry )
{
    StreamMap::iterator  sIter;

    if ( ! Logger::Lock() )
        return;

    sIter = strmMap.find(streamName);
    if ( sIter == strmMap.end() )
        return;

    *(sIter->second) << prefix << ": ";
    if ( Logger::logTime > 0 )
        *(sIter->second) << Logger::logTimeStr << " : ";
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

    Logger::enabled = false;

    strmMap.clear();

    return;
}


void
Logger::CloseSyslog()
{
#   ifndef WIN32
    if ( syslog ) {
        ::syslog(LOGGER_NOTICE, "==== Logger Closing ====");
        ::closelog();
        syslog = false;
    }
#   endif

    if ( logStream == NULL )
        enabled = false;

    return;
}


void
Logger::CloseFilelog()
{
    std::ofstream  * fstrm = NULL;

    Logger::LogMessage("==== Logger Closing ====");

    fstrm = (std::ofstream*) Logger::RemoveStream(Logger::fileName);

    if ( fstrm != NULL && fstrm == logStream ) {
        fstrm->close();
        delete logStream;
        logStream = NULL;
        fileName  = "";
    }

    return;
}

// ----------------------------------------------------------------------

void
Logger::SetLogPrefix ( const std::string & prefix )
{
    if ( ! prefix.empty() )
        logPrefix = prefix;
    return;
}

std::string
Logger::GetLogPrefix()
{
    return logPrefix;
}

// ----------------------------------------------------------------------

void
Logger::SetLogTime ( const time_t & now )
{
    if ( Logger::Lock() ) {
        Logger::logTime  = now;
    
        if ( Logger::logTime > 0 )
            Logger::GetTimeStr(now, Logger::logTimeStr);
        Logger::Unlock();
    }

    return;
}

// ----------------------------------------------------------------------

void
Logger::GetTimeStr ( const time_t & now, std::string & timestr )
{
    char  tstr[32];

#   ifdef WIN32
    char * tptr = ::ctime(&now);
    ::strncpy(tstr, tptr, 32);
#   else
    ::ctime_r(&now, tstr);
#   endif
    timestr = tstr;
    timestr = timestr.substr(0, timestr.length() - 1);

    return;
}

// ----------------------------------------------------------------------

void
Logger::InitLogMsg()
{
    LoggerMsg    logMsg;
    std::string  tstr;
    time_t       now;

    now  = ::time(NULL);
    Logger::GetTimeStr(now, tstr);

    logMsg << "Log Initialized at '" << tstr << "'";

    Logger::LogMessage(logMsg.str());
}

// ----------------------------------------------------------------------

bool
Logger::Lock()
{ 
#   ifndef WIN32
    if ( Logger::initlock && Logger::trylock ) {
        if ( Logger::lock.tryLock() <= 0 )
            return false;
    } else if ( Logger::initlock ) {
        if ( Logger::lock.lock() <= 0 )
            return false;
    }
#   endif

    return true;
}

void
Logger::Unlock()
{
#   ifndef WIN32
    if ( Logger::initlock )
        Logger::lock.unlock();
#   endif 
    return;
}

// ----------------------------------------------------------------------


} // namespace


//  _TCANETPP_LOGGER_CPP_ 

