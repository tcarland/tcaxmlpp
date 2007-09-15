/** 
  *   Logger - Logging facility supporting multiple syslog(unix), filelog, or
  *            log iostreams.
  * 
  *   @Author  tcarland@gmail.com 
 **/
#ifndef _TCANETPP_LOGGER_H_
#define _TCANETPP_LOGGER_H_

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


typedef enum LoggerLevel
{
    LOGGER_EMERG   = 0,
    LOGGER_ALERT   = 1, 
    LOGGER_ERR     = 3,
    LOGGER_WARN    = 4,
    LOGGER_NOTICE  = 5,
    LOGGER_DEBUG   = 7
} logLevel_t;



typedef std::ostringstream    LoggerMsg;


class Logger {

  public:

    /**  Initializes a thread-safe version of the logging facility. Once this function 
      *  is called the LogMessage routines will lock for thread safety. This should be 
      *  called by a threaded app in addition to the normal Init functions. 
      *  Note that all Init/Open/Close functions are not threadsafe and should only be 
      *  called by the main thread. This is not supported in WIN32 at this time.
      *  
      *  @ param  trylock  if set to true will enable a non-blocking trylock for all 
      *                    threads. Locks that fail to acquire will silently return
      *                    without logging a message.
     **/
    static bool           InitThreaded ( bool trylock = false );
    

    /**  Opens a log stream of the provided filename  */
    static bool           OpenLogFile ( const std::string & prefix,
                                        const std::string & filename, bool append = true );

    /**   Enables the syslog Log Stream on unix systems. If this is called in a 
      *   WIN32 environment, it will return false.
     **/
    static bool           OpenSyslog ( const std::string & prefix, int facility );
    
    /**   Enables the provided log stream.  This simply wraps AddStream(), 
      *   and the provided ostream pointer is NOT deleted on close
     **/
    static bool           OpenLogStream ( const std::string & name, std::ostream * stream );
    
    
    /**  Adds a stream object to the list of log streams. Streams are tracked by the 
      *  identifying name or prefix, and any streams added via this method are not deleted 
      *  leaving it to the application to properly remove streams (see RemoveStream).
      * 
      *  @ param name    is a unique string identifying the stream object.
      *  @ param stream  is a pointer to a STL ostream object.
     **/
    static bool           AddStream    ( const std::string & name, std::ostream * stream );

    
    /**  RemoveStream removes any references to this stream, and returns the associated
      *  stream pointer.  This should be used on any heap allocated stream object.
      *  Consider the following:
      *
      *      Logger::AddStream("mystream", ( (std::ostream*) new std::ofstream(filename) ));
      *      
      *  leaves the only valid pointer within the Logger, requiring the use of RemoveStream
      *  to cleanup:
      *  
      *      std::ofstream * fstrm = (std::ofstream*) Logger::RemoveStream("mystream");
      *      if ( fstrm ) {  fstrm->close();  delete fstrm;  }
      * 
     **/
    static std::ostream*  RemoveStream ( const std::string & name );
    
    \
    /**  Closes all logs and clears stream map WITHOUT deleting any ostreams
      *  (ie. applications responsibility to clean up any heap allocated stream objects)
      *  the only exception being a file logger created via OpenFileLog().
     **/
    static void           CloseLogger();

    
    /**  Logs the message to the default or primary log stream  */
    static void           LogMessage   ( const std::string & entry, 
                                         int level = LOGGER_NOTICE );

    static void           LogMessage   ( const std::string & prefix, 
                                         const std::string & entry,
                                         int level = LOGGER_NOTICE );

    /**  Logs the message to a specific stream */
    static void           LogToStream  ( const std::string & streamName,
                                         const std::string & prefix,
                                         const std::string & entry );

    static void           LogToStream  ( const std::string & streamName,
                                         const std::string & entry );

    /*  Broadcast the message to all log streams */
    static void           LogToAllStreams ( const std::string & prefix,
                                            const std::string & entry );
                                        
    
    /**  Closes down the syslog handle if enabled */
    static void           CloseSyslog();
    
    /**  Closes down the primary filelog handle if enabled */
    static void           CloseFilelog();
    
    /**  Closes down all log handles. Note: Call RemoveStream() first */
    static inline void    Close()      { return Logger::CloseLogger(); }


    /**  Sets an approximate log time to use for a subsequent log messages
      *  This should be set on a timer of at least a few minutes.
     **/
    static void           SetLogTime   ( const time_t & now );
  
    /**  Creates a formatted text string from the given time value and populates the 
      *  provided @param timestr.
     **/
    static void           GetTimeStr   ( const time_t & now, std::string & timestr );


    static void           SetEnabled   ( bool enable );
    static bool           GetEnabled();
    static inline bool    Enabled()    { return Logger::GetEnabled(); }

    /**  Sets a default log prefix to be used when no prefix is provided */
    static void           SetLogPrefix ( const std::string & prefix );
    static std::string    GetLogPrefix();

    /**  Indicates whether a primarly log handle is valid and enabled (syslog or filelog).
      *  Additional streams are not considered in this evaluation.
     **/
    static bool           IsOpen();

    static void           SetDebug     ( bool debug );
    static bool           Debug();

    static std::string    GetFileName();


  private:

    typedef std::map<std::string, std::ostream*>  StreamMap;

    static bool           InitSyslog   ( int facility );
    static void           InitLogMsg();

    static bool           Lock();
    static void           Unlock();


  private:

    static StreamMap             strmMap;

#   ifndef WIN32
    static tcanetpp::ThreadLock  lock;
#   endif

    static std::string           logPrefix;
    static std::string           fileName;
    static std::string           logTimeStr;
    static std::ostream *        logStream;
    static time_t                logTime;
    static bool                  init;
    static bool                  initlock;
    static bool                  trylock;
    static bool                  enabled;
    static bool                  syslog;
    static bool                  debug;

};


} // namespace

#endif // _TCANETPP_LOGGER_H_
