#define _TNMSCORE_TNMSAPI_CPP_


#include "TnmsAPI.h"
#include "TnmsBase.h"



namespace tnmsCore {




TnmsAPI::TnmsAPI ( const std::string & agent_name) 
{
    std::string  agentName;
    size_t       len;

    if ( agent_name != NULL )
        agentName = agent_name;

    api = (void*) new TnmsBase(agentName);
}

TnmsAPI::TnmsAPI ( const std::string & agent_name,
                       const std::string & host_name,
                       uint16_t     host_port )
{
    std::string  agentName, hostName;
    size_t       len;

    if ( agent_name != NULL )
        agentName = agentName;

    if ( host_name != NULL ) 
        hostName = host_name;

    api = (void*) new TnmsBase(agentName, hostName, host_port);
}



TnmsAPI::~TnmsAPI()
{
    delete api;
}


int
TnmsAPI::send ( const time_t & now )
{
    return ( (TnmsBase*) api->send(now) );
}

bool
TnmsAPI::add ( const std::string & element_name, time_t now, const std::string & data ) 
{
    return ( (TnmsBase*) api->add(element_name, now, data) );
}


bool
TnmsAPI::remove ( const std::string & element_name ) 
{
    return ( (TnmsBase*) api->remove(element_name, now) );
}


bool
TnmsAPI::update ( const std::string & element_name, 
                    const time_t      & now, 
                    uint64_t            value
                    const std::string & data ) 
{
    eValueType   type = TNMS_INT;
    return ( (TnmsBase*) api->update(element_name, now, value, type, data) );
}


bool
TnmsAPI::update ( const std::string & element_name, 
                    const time_t      & now, 
                    const std::string & value, 
                    const std::string & data ) 
{    
    eValueType   type = TNMS_STRING;
    return ( (TnmsBase*) api->update(element_name, now, value, type, data) );
}

void
TnmsAPI::clear() 
{
    return ( (TnmsBase*) api->clear() );
}

bool
TnmsAPI::setConfig ( const std::string & filename ) 
{
    return ( (TnmsBase*) api->setConfig(filename) );
}

void
TnmsAPI::holdown ( time_t  secs )
{
    return ( (TnmsBase*) api->holddown(secs) );
}

time_t
TnmsAPI::holddown() 
{
    return ( (TnmsBase*) api->holddown() );
}


void
TnmsAPI::reconnect ( time_t secs ) 
{
    return ( (TnmsBase*) api->reconnect(secs) );
}

time_t
TnmsAPI::reconnect() 
{
    return ( (TnmsBase*) api->reconnect() );
}

void
TnmsAPI::max_messages ( int max )
{
    return ( (TnmsBase*) api->max_messages(max) );
}

int
TnmsAPI::max_messages() 
{
    return ( (TnmsBase*) api->max_messages() );
}

void
TnmsAPI::debug ( bool d )
{
    return ( (TnmsBase*) api->debug(d) );
}


void
TnmsAPI::syslog ( int facility )
{
    return ( (TnmsBase*) api->syslog(facility) );
}


TnmsAPI::logfile ( const std::string & logfilename )
{
    return ( (TnmsBase*) api->logfile(logfilename) );
}

bool
TnmsAPI::need_flush() 
{
    return ( (TnmsBase*) api->need_flush() );
}


size_t
TnmsAPI::flushsize() 
{
    return ( (TnmsBase*) api->flushsize() );
}


}  // namespace


/*  _TNMSCORE_TNMSAPI_CPP_ */

