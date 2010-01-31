#define _TNMSCORE_TNMSAPI_CPP_


#include "TnmsAPI.h"
#include "TnmsBase.h"


namespace tnmsApi {



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
    api = (void*) new TnmsBase(agent_name, host_name, host_port);
}



TnmsAPI::~TnmsAPI()
{
    if ( api )
        delete (TnmsBase*) api;
}


/**   send()  gives time to the the API. The API instance will only send
  * data when this method is called.  The API is completely non-blocking
  * and should be called as frequently as possible by the owning 
  * application's main loop (at least once a second if not faster).  The 
  * API will only trigger an interval of sending all updates based on 
  * the 'holddown' attribute.  For example, if the holddown is set to 30
  * seconds, then all updates within the 30 second interval are aggregated into 
  * one update for the interval, sent when the holddown timer expires.
  * Calls to send() within the window ensure that all previous updates from the 
  * last interval have been flushed. If send() is called too infrequently, 
  * then API will potentially not have sufficient time to flush all of updates.
 **/ 
int
TnmsAPI::send ( const time_t & now )
{
    return ( ((TnmsBase*)api)->send(now) );
}


/**  Adds a new element to the API. The API automatically prepends the 
  *  agent name to the element. The method can fail if the element 
  *  already exists, or for some reason the underlying tree insert 
  *  failed for some reason (unlikely).
 **/
bool
TnmsAPI::add ( const std::string & element_name, const time_t & now ) 
{
    return ( ((TnmsBase*)api)->add(element_name, now) );
}


/**  Removes a given element from the API */
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

