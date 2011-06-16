/**
  *  PollerManager
  * 
 **/
#ifndef _POLLAP_POLLERMANAGER_H_
#define _POLLAP_POLLERMANAGER_H_

#include <map>

#include "EventManager.h"
using namespace tcanetpp;

#include "PollerConfig.h"


namespace Polheir {


#define LOGCHECK_INTERVAL 3600



class PollerManager : public TimerHandler {

  public:

    PollerManager();
    PollerManager ( const std::string & configxml );
    PollerManager ( const std::string & queryxml,
                    const std::string & devicexml );

    virtual ~PollerManager();


    void         run();
    void         timeout        ( const EventTimer * timer );
    bool         runQuery       ( PollerDevice & device, PollerQuery & query );

    
  protected:

    std::string  oidHasKey      ( const std::string & queryoid,
                                  const std::string & respoid );

    int          getDeviceSlot  ( PollerDevice      & dev,
                                  const std::string & queryName );

    void         runSubQueries  ( SnmpSession & snmp, PollerQuery & query,
                                  std::string & soid, int slot );

    void         setQueryResult ( SnmpSession & snmp, PollerQuery & query,
                                  struct snmp_pdu * response );

    void         showResult     ( SnmpPdu * response );
    bool         parseConfig    ( const time_t & now );

    void         logRotate ( std::string logfile, const time_t & now );


  private:

    EventManager *              _evmgr;

    std::string                 _querycfg;
    std::string                 _devicecfg;

    PollerQueryMap              _queries;
    PollerDeviceList            _devlist;
   
    std::string                 _errstr;
    time_t                      _lastTouched;
    time_t                      _logRotate;
    int                         _currentDay;
    bool                        _hup, _verbose;

};


} // namespace

#endif  // _POLLAP_POLLERMANAGER_H_
