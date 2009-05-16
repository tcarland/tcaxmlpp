#define _TNMSCORE_TNMSAPI_CPP_


#include "TnmsAPI.h"
#include "TnmsBase.h"


namespace tnmsCore {




TnmsAPI::TnmsAPI ( const std::string & agent_name ) 
{
    std::string  agentName;

    if ( ! agent_name.empty() )
        agentName = agent_name;

    api = (void*) new TnmsBase(agentName);
}

TnmsAPI::TnmsAPI ( const std::string & agent_name,
                   const std::string & host_name,
                   uint16_t            host_port )
{
    std::string  agentName, hostName;

    if ( ! agent_name.empty() )
        agentName = agentName;

    if ( ! host_name.empty() ) 
        hostName = host_name;

    api = (void*) new TnmsBase(agentName, hostName, host_port);
}



TnmsAPI::~TnmsAPI()
{
    if ( api )
        delete (TnmsBase*) api;
}


int
TnmsAPI::send ( const time_t & now )
{
    return ( ((TnmsBase*)api)->send(now) );
}


bool
TnmsAPI::add ( const std::string & element_name, const time_t & now ) 
{
    return ( ((TnmsBase*)api)->add(element_name, now) );
}


bool
TnmsAPI::add ( const std::string & element_name, const time_t & now, const std::string & data ) 
{
    return ( ((TnmsBase*)api)->add(element_name, now, data) );
}


bool
TnmsAPI::remove ( const std::string & element_name ) 
{
    return ( ((TnmsBase*)api)->remove(element_name) );
}


bool
TnmsAPI::update ( const std::string & element_name, 
                  const time_t      & now, 
                  uint64_t          & value,
                  eValueType          type ) 
{
    return ( ((TnmsBase*)api)->update(element_name, now, value, type) );
}


bool
TnmsAPI::update ( const std::string   & element_name, 
                    const time_t      & now, 
                    const std::string & value ) 
{    
    return ( ((TnmsBase*)api)->update(element_name, now, value) );
}


bool
TnmsAPI::update ( const std::string  & element_name,
                  const std::string  & data )
{
    return ( ((TnmsBase*)api)->update(element_name, data) );
}


void
TnmsAPI::clear() 
{
    return ( ((TnmsBase*)api)->clear() );
}

void
TnmsAPI::set_config ( const std::string & filename ) 
{
    return ( ((TnmsBase*)api)->set_config(filename) );
}

void
TnmsAPI::holddown_interval ( time_t  secs )
{
    return ( ((TnmsBase*)api)->holddown_interval(secs) );
}

time_t
TnmsAPI::holddown_interval() 
{
    return ( ((TnmsBase*)api)->holddown_interval() );
}


void
TnmsAPI::reconnect_interval ( time_t secs ) 
{
    return ( ((TnmsBase*)api)->reconnect_interval(secs) );
}

time_t
TnmsAPI::reconnect_interval() 
{
    return ( ((TnmsBase*)api)->reconnect_interval() );
}

void
TnmsAPI::max_messages ( int max )
{
    return ( ((TnmsBase*)api)->flush_limit(max) );
}

int
TnmsAPI::max_messages() 
{
    return ( ((TnmsBase*)api)->flush_limit() );
}

void
TnmsAPI::set_debug ( bool d )
{
    return ( ((TnmsBase*)api)->set_debug(d) );
}


void
TnmsAPI::set_syslog ( int facility )
{
    return ( ((TnmsBase*)api)->set_syslog(facility) );
}


void
TnmsAPI::set_logfile ( const std::string & logfilename )
{
    return ( ((TnmsBase*)api)->set_logfile(logfilename) );
}

bool
TnmsAPI::need_flush() 
{
    return ( ((TnmsBase*)api)->need_flush() );
}


size_t
TnmsAPI::flushsize() 
{
    return ( ((TnmsBase*)api)->flushsize() );
}


}  // namespace


/*  _TNMSCORE_TNMSAPI_CPP_ */

