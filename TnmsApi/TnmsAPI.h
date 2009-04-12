#ifndef _TNMSCORE_TNMSAPI_H_
#define _TNMSCORE_TNMSAPI_H_

#include <stdint.h>

#include <string>

#include "tnmsCore.h"


#ifndef DLLOPT
#  if defined (WIN32) && ! defined (TNMSAPI_NODLL)
#    ifdef EXPORT_DLL
#      define DLLOPT __declspec(dllexport)
#    else
#      define DLLOPT  __declspec(dllimport)
#    endif
#  else
#    define DLLOPT
#  endif
#endif



namespace tnmsCore {



class DLLOPT TnmsAPI {

  public:

    TnmsAPI ( const std::string & agent_name );

    TnmsAPI ( const std::string & agent_name,
              const std::string & host_name,
              uint16_t            host_port );

    ~TnmsAPI();


    int     send    ( const time_t  & now );


    bool    add     ( const std::string & element_name, 
                      const time_t      & now,
                      const std::string & data = 0 );

    bool    remove  ( const std::string & element_name );

    bool    update  ( const std::string & element_name, 
                      const time_t      & now, 
                      uint64_t          & value,
                      eValueType          type,
                      const std::string & data = 0 );

    bool    update  ( const std::string & element_name, 
                      const time_t      & now,
                      const std::string & value, 
                      const std::string & data = 0 );


    void    clear();


    void    holddown_interval  ( time_t  secs );
    time_t  holddown_interval();

    void    reconnect_interval ( time_t  secs );
    time_t  reconnect_interval();

    void    max_messages ( int  max );
    int     max_messages();

    void    set_config ( const std::string & filename );
    void    set_debug   ( bool debug );
    void    set_syslog  ( int facility );
    void    set_logfile ( const std::string & logfilename );

    bool    need_flush();
    size_t  flushsize();

    static 
    const std::string&  Version();

  private:

    void   *  api;
};


}  // namespace

#endif  // _TNMSCORE_TNMSAPI_H_

