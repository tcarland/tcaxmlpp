#define _TNMSCORE_TNMSAPI_CPP_


#include "TnmsAPI.h"
#include "TnmsBase.h"


namespace tnmsApi {


/**  Constructor for instantiating a new agent(api) instance*/
TnmsAPI::TnmsAPI ( const std::string & agent_name ) 
{
    std::string  agentName;

    if ( ! agent_name.empty() )
        agentName = agent_name;

    api = (void*) new TnmsBase(agentName);
}

/**  Constructor for instantiating a new agent(api) instance
  * to the provided server host:port.
  *
  * @param agent_name  is the initial 'unique' name of this agent.
  * @param host_name   is the hostname of the server to connect.
  * @param host_port   is the port number of the server.
 */
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
  * and should be called as frequently as possible by the application's
  * main loop (at least once a second if not faster).  The
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

/**@
  *  Update an element where the @param element_name represents
  *  a non-string value type. The value provided to the api is 
  *  always an unsigned 64-bit integer, and any desired type 
  *  should simply be cast to this type which can be reinterpreted 
  *  by the client based on @param type. 
  *  The supported types are based on the enum tnmsCore::eValueType
  *  with the following types:
  *
  *  typedef enum eValueType {
  *      TNMS_NONE    = 0,
  *      TNMS_INT32   = 1,
  *      TNMS_UINT32  = 2,
  *      TNMS_INT64   = 3,
  *      TNMS_UINT64  = 4,
  *      TNMS_FLOAT   = 5,
  *      TNMS_STRING  = 6 
  *  } e_valueType_t;
  *
 **/
bool
TnmsAPI::update ( const std::string & element_name, 
                  const time_t      & now, 
                  uint64_t          & value,
                  uint16_t            type ) 
{
    return ( ((TnmsBase*)api)->update(element_name, now, value, type) );
}


/**  Updates @param element_name to the provided string value */
bool
TnmsAPI::update ( const std::string   & element_name, 
                    const time_t      & now, 
                    const std::string & value ) 
{    
    return ( ((TnmsBase*)api)->update(element_name, now, value) );
}


/**  Update the atomic data element to be carried with the key/value 
  *  pair defined by @param element_name
 **/
bool
TnmsAPI::update ( const std::string  & element_name,
                  const std::string  & data )
{
    return ( ((TnmsBase*)api)->update(element_name, data) );
}


/**  Clears the current tree of all metrics */
void
TnmsAPI::clear() 
{
    return ( ((TnmsBase*)api)->clear() );
}

/**  Convenience method to set/change the running api config */
void
TnmsAPI::set_config ( const std::string & filename ) 
{
    return ( ((TnmsBase*)api)->set_config(filename) );
}


/**  Convenience method to set the hold-down interval of the 
  *  running api instance. 
  *  @param  secs   defines the number of secconds to gather
  *  updates for reporting. Only updated elements are sent
  *  once this interval has expired. All subsequent sends 
  *  will always send out any queued or waiting messages 
  *  whose holddown_interval has expired. 
 **/
void
TnmsAPI::holddown_interval ( time_t  secs )
{
    return ( ((TnmsBase*)api)->holddown_interval(secs) );
}

/** Return the configured hold-down interval in seconds */
time_t
TnmsAPI::holddown_interval() 
{
    return ( ((TnmsBase*)api)->holddown_interval() );
}

/** Interval to delay reconnect attempts */
void
TnmsAPI::reconnect_interval ( time_t secs ) 
{
    return ( ((TnmsBase*)api)->reconnect_interval(secs) );
}

/** Return the configured reconnect interval in seconds */
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

