#define _TNMSCORE_TNMSAPI_CPP_


#include "TcaNmsAPI.h"
#include "TcaNmsBase.h"




namespace tnmsCore {




TcaNmsAPI::TcaNmsAPI ( const std::string & agent_name) 
{
    std::string  agentName;
    size_t       len;

    if ( agent_name != NULL )
        agentName = agent_name;

    api = (void*) new TcaNmsBase(agentName);
}

TcaNmsAPI::TcaNmsAPI ( const std::string & agent_name,
                       const std::string & host_name,
                       uint16_t     host_port )
{
    std::string  agentName, hostName;
    size_t       len;

    if ( agent_name != NULL )
        agentName = agentName;

    if ( host_name != NULL ) 
        hostName = host_name;

    api = (void*) new TcaNmsBase(agentName, hostName, host_port);
}



TcaNmsAPI::~TcaNmsAPI()
{
    delete api;
}


int
TcaNmsAPI::send ( const time_t & now )
{
    return ( (TcaNmsBase*) api->send(now) );
}

bool
TcaNmsAPI::add ( const std::string & element_name, time_t now, const std::string & data ) 
{
    return ( (TcaNmsBase*) api->add(element_name, now, data) );
}


bool
TcaNmsAPI::remove ( const std::string & element_name ) 
{
    return ( (TcaNmsBase*) api->remove(element_name, now) );
}


bool
TcaNmsAPI::update ( const std::string & element_name, 
                    const time_t      & now, 
                    uint64_t            value
                    const std::string & data ) 
{
    eValueType   type = TNMS_INT;
    return ( (TcaNmsBase*) api->update(element_name, now, value, type, data) );
}


bool
TcaNmsAPI::update ( const std::string & element_name, 
                    const time_t      & now, 
                    const std::string & value, 
                    const std::string & data ) 
{    
    eValueType   type = TNMS_STRING;
    return ( (TcaNmsBase*) api->update(element_name, now, value, type, data) );
}

void
TcaNmsAPI::clear() 
{
    return ( (TcaNmsBase*) api->clear() );
}

bool
TcaNmsAPI::setConfig ( const std::string & filename ) 
{
    return ( (TcaNmsBase*) api->setConfig(filename) );
}

void
TcaNmsAPI::holdown ( time_t  secs )
{
    return ( (TcaNmsBase*) api->holddown(secs) );
}

time_t
TcaNmsAPI::holddown() 
{
    return ( (TcaNmsBase*) api->holddown() );
}


void
TcaNmsAPI::reconnect ( time_t secs ) 
{
    return ( (TcaNmsBase*) api->reconnect(secs) );
}

time_t
TcaNmsAPI::reconnect() 
{
    return ( (TcaNmsBase*) api->reconnect() );
}

void
TcaNmsAPI::max_messages ( int max )
{
    return ( (TcaNmsBase*) api->max_messages(max) );
}

int
TcaNmsAPI::max_messages() 
{
    return ( (TcaNmsBase*) api->max_messages() );
}

void
TcaNmsAPI::debug ( bool d )
{
    return ( (TcaNmsBase*) api->debug(d) );
}


void
TcaNmsAPI::syslog ( int facility )
{
    return ( (TcaNmsBase*) api->syslog(facility) );
}


TcaNmsAPI::logfile ( const std::string & logfilename )
{
    return ( (TcaNmsBase*) api->logfile(logfilename) );
}

bool
TcaNmsAPI::need_flush() 
{
    return ( (TcaNmsBase*) api->need_flush() );
}


size_t
TcaNmsAPI::flushsize() 
{
    return ( (TcaNmsBase*) api->flushsize() );
}


}  // namespace


/*  _TNMSCORE_TNMSAPI_CPP_ */

