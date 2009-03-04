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
                  eValueTypes         type,
                  const std::string & data ) 
{
    return ( ((TnmsBase*)api)->update(element_name, now, value, type, data) );
}


bool
TnmsAPI::update ( const std::string   & element_name, 
                    const time_t      & now, 
                    const std::string & value, 
                    const std::string & data ) 
{    
    return ( ((TnmsBase*)api)->update(element_name, now, value, data) );
}

void
TnmsAPI::clear() 
{
    return ( ((TnmsBase*)api)->clear() );
}

bool
TnmsAPI::setConfig ( const std::string & filename ) 
{
    return ( ((TnmsBase*)api)->setConfig(filename) );
}

void
TnmsAPI::holddown ( time_t  secs )
{
    return ( ((TnmsBase*)api)->holddown(secs) );
}

time_t
TnmsAPI::holddown() 
{
    return ( ((TnmsBase*)api)->holddown() );
}


void
TnmsAPI::reconnect ( time_t secs ) 
{
    return ( ((TnmsBase*)api)->reconnect(secs) );
}

time_t
TnmsAPI::reconnect() 
{
    return ( ((TnmsBase*)api)->reconnect() );
}

void
TnmsAPI::max_messages ( int max )
{
    return ( ((TnmsBase*)api)->max_messages(max) );
}

int
TnmsAPI::max_messages() 
{
    return ( ((TnmsBase*)api)->max_messages() );
}

void
TnmsAPI::debug ( bool d )
{
    return ( ((TnmsBase*)api)->debug(d) );
}


void
TnmsAPI::syslog ( int facility )
{
    return ( ((TnmsBase*)api)->syslog(facility) );
}


void
TnmsAPI::logfile ( const std::string & logfilename )
{
    return ( ((TnmsBase*)api)->logfile(logfilename) );
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

