/**
  *  PollerManager
  * 
  *  Copyright(c) 2006, Sony Online Entertainment Inc.
  *  All rights reserved.
  *
  *  @Author   tca@sonyonline.net
 **/
#define _POLLAP_POLLERMANAGER_CPP_

#include <iostream>


#include "PollerManager.h"

#include "Logger.h"
#include "StringUtils.h"
using namespace tca::net;


namespace Polheir {


PollerManager::PollerManager()
    : _evmgr(NULL),
      _lastTouched(0),
      _logRotate(0),
      _currentDay(0),
      _hup(true),
      _verbose(false)
{}

PollerManager::PollerManager ( const std::string & queryxml )
    : _evmgr(new EventManager()),
      _querycfg(queryxml),
      _lastTouched(0),
      _logRotate(0),
      _currentDay(0),
      _hup(true),
      _verbose(false)
{}

PollerManager::PollerManager ( const std::string & queryxml, 
                               const std::string & devicexml )
    : _evmgr(new EventManager()),
      _querycfg(queryxml),
      _devicecfg(devicexml),
      _lastTouched(0),
      _logRotate(0),
      _currentDay(0),
      _hup(true),
      _verbose(false)
{}

PollerManager::~PollerManager()
{
    if ( _evmgr )
        delete _evmgr;
}



void
PollerManager::run()
{
    time_t  now = ::time(NULL);

    Logger::SetLogTime(now);

    if ( _verbose )
        Logger::InitLogger("stdout", &std::cout);

    _logRotate    = now + LOGCHECK_INTERVAL;
    //_metricUpdate = now + (METRIC_UPDATE_INTERVAL * 2);

    // create timer event for config checking 
    _evmgr->addTimerEvent((TimerHandler*) this, 5, 0);

    if ( ! this->parseConfig(now) ) {
        printf("ERROR: %s\n", _errstr.c_str());
        Logger::LogMessage(_errstr);
        return;
    }

    _evmgr->eventLoop();
    Logger::Close();

    return;
}


void
PollerManager::timeout ( const EventTimer * timer )
{
    const time_t & now = timer->abstime.tv_sec;

    Logger::SetLogTime(now);
    
    if ( _hup )
        this->parseConfig(now);

    std::string  log = "PollAp";

    if ( _logRotate <= timer->abstime.tv_sec )
        this->logRotate(log, timer->abstime.tv_sec);

    PollerDeviceList::iterator  dIter;

    for ( dIter = _devlist.begin(); dIter != _devlist.end(); ++dIter ) {
        PollerDevice    & device   = *dIter;
        DeviceQueryMap  & queryMap = dIter->queryMap;
        
        DeviceQueryMap::iterator  qIter;
        PollerQueryMap::iterator  sIter;

        Logger::LogMessage("Running queries for " +  dIter->name);

        for ( qIter = queryMap.begin(); qIter != queryMap.end(); ++qIter ) {
            const std::string & qname = qIter->first;
            PollerDeviceQuery & devq  = qIter->second;

            if ( (devq.last_query + devq.update_interval) > now )
                continue;

            sIter = _queries.find(qname);

            if ( sIter == _queries.end() ) {
                Logger::LogMessage("No query found for " + qname);
                continue;
            }

            if ( this->runQuery(*dIter, sIter->second) )
                devq.last_query = now;
        }

    }

    return;
}

int
PollerManager::getDeviceSlot ( PollerDevice & device, const std::string & queryName )
{
    int  slot = 0;

    DeviceQueryMap::iterator  dIter;
    
    dIter = device.queryMap.find(queryName);

    if ( dIter != device.queryMap.end() )
        slot = dIter->second.slot_number;

    return slot;
}


bool
PollerManager::runQuery ( PollerDevice & device, PollerQuery & query )
{
    SnmpSession      snmp;
    SnmpPdu        * response = NULL;
    int              slot     = 0;
    std::string      oidstr, respoid, keystr, keyoid;
    LoggerMsg        msg;

    msg << "runQuery() " << device.name << " : " << query.name;
    Logger::LogMessage(msg.str());

    std::cout << "runQuery() " << query.query_type
              << " query: " << query.oid.getOidStr() << std::endl;

    if ( query.query_type == SNMPQUERY_TYPE_GETNEXT ) {

        oidstr = query.oid.getOidStr();

        if ( query.use_slot_num ) {
            slot = this->getDeviceSlot(device, query.name);

            if ( slot == 0 ) {
                std::cout << "Invalid slot number" << std::endl;
                return false;
            }
            oidstr.append(".");
            oidstr.append(StringUtils::toString<int>(slot));
        }

        response = snmp.snmpGetNext(query.oid.getOidStr().c_str());

        while ( response ) {
            response->getOid(respoid);
            
            std::cout << "response oid:" << respoid << std::endl;

            if ( query.oid_is_key )
                keystr = this->oidHasKey(oidstr, respoid);
               
            keyoid  = SnmpOid::StringToBitString(keystr);

            if ( query.subQueries.size() > 0 )
                this->runSubQueries(snmp, query, keyoid, slot);

            //std:: cout << " status = ";
            //this->showResult(response);

            response = snmp.snmpGetNext(response);
        }
        
    } else if ( query.query_type == SNMPQUERY_TYPE_GET ) {
        std::cout << " query " << query.oid.getOidStr() << std::endl;

        response = snmp.snmpGet(query.oid.getOidStr().c_str());

        if ( response ) {
            this->showResult(response);
        }

    }
     
        
    return true;
}

void
PollerManager::runSubQueries ( SnmpSession & snmp, PollerQuery & query,
                               std::string & soid, int slot ) 
{
    SnmpPdu * pdu  = NULL;
    
    PollerQueryList::iterator qIter;

    for ( qIter = query.subQueries.begin(); qIter != query.subQueries.end(); ++qIter ) {
    }

    return;
}


std::string
PollerManager::oidHasKey ( const std::string & queryoid, const std::string & respoid )
{
    return SnmpOid::StringFromBitString(respoid.substr(queryoid.length()+1));
}


void
PollerManager::showResult ( SnmpPdu * response )
{
    std::string  foo;
    char  t;
    int   type;
    int   f   = 0;

    response->getResponseType(t);
    type = (int) t;

    switch ( t ) {
        case ASN_OCTET_STR:
            response->getString(foo);
            std::cout  << foo;
            break;
        case ASN_INTEGER:
            response->getInteger(f);
            std::cout <<  f;
            break;
        case ASN_OBJECT_ID:
            response->getObjectId(foo);
            std::cout << foo;
            break;
        default:
            response->getString(foo);
            std::cout << foo;
            break;
    }

    return;
}


bool
PollerManager::parseConfig ( const time_t & now )
{
    if ( _querycfg.empty() || _devicecfg.empty() )
        return false;

    _devlist.clear();
    _queries.clear();

    PollerConfig    pcfg;

    if ( ! pcfg.parseDeviceConfig(_devicecfg, _devlist) )
        return false;

    if ( ! pcfg.parseQueryConfig(_querycfg, _queries) )
        return false;

    return true;
}


void
PollerManager::logRotate ( std::string logfile, const time_t & now )
{
    char       datestr[64];
    struct tm  *t     = ::localtime(&now);
    int         today = t->tm_yday;

    if ( logfile.empty() ) 
        return;

    if ( _currentDay != today ) {
        ::strftime(datestr, 64, ".%Y%02m%02d", t);
        logfile.append(datestr);
        Logger::OpenLogFile(logfile, true);
        _currentDay = today;
    }

    _logRotate = now + LOGCHECK_INTERVAL;

    return;
}



} // namespace

//  _POLLAP_POLLERMANAGER_CPP_

