
#include <iostream>

#include "FwLogReport.h"

#include "StringUtils.h"
#include "LogFacility.h"
using namespace tcanetpp;

#include "tnmsCore.h"
using namespace tnmsCore;


namespace fwgen {



FwLogReport::FwLogReport ( const std::string & agent, const std::string & host, uint16_t port )
    : _connection(false)
{
    _api = new TnmsAPI(agent, host, port);
    FwService::ParseServices(ETC_SERVICES, this->_svcMap);
}

FwLogReport::FwLogReport ( const std::string & config )
    : _connection(false)
{
    _api = new TnmsAPI("fwlr");
    _api->set_config(config);
    FwService::ParseServices(ETC_SERVICES, this->_svcMap);
}


int
FwLogReport::FlushApi ( const time_t & now )
{
    int  retval = 0;
    int  errcnt = 0;

    if ( ! _connection )
        LogFacility::LogMessage("FwLogReport::FlushApi() no API connection, attempting reconnect");

    do {
        retval = _api->send(now);
        
        if ( retval == 1 ) {
            LogFacility::LogMessage("FwLogReport::FlushApi(): API has Invalid configuration");
            return retval;
        }

        if ( retval > 0 )
            retval = _api->send(now + 1);

        if ( retval > 0 ) {
            if ( ! _connection ) {
                std::cout << ".";
                std::cout.flush();
                sleep(1);
            } else if ( retval == TNMSERR_CONN_DENIED ) {
                LogFacility::LogMessage("  FwLogReport::FlushApi() Not authorized.");
                _connection = false;
                break;
            }
            errcnt++;
        } else {
            if ( ! _connection )
                LogFacility::LogMessage("  FwLogReport::FlushApi(): Connected.");
            _connection = true;
        }
    } while ( retval > 0 && errcnt < 8 );

    if ( ! _connection )
        LogFacility::LogMessage("  FwLogReport::FlushApi(): Connection failed");

    return retval;
}


void
FwLogReport::SendEntry ( FwLogEntry & fwe, const time_t & now )
{
    FwMap::iterator    fIter;
    FwSvcMap::iterator sIter;

    if ( fwe.inf.empty() )
        return;

    //  src <=> dst  
    fwe.absname = fwe.inf;
    fwe.absname.append("/").append(fwe.src);
    // 
    //fwe.absname.append("<->").append(fwe.dst);
    fwe.protom  = fwe.absname;

    int srcp = StringUtils::fromString<int>(fwe.spt);
    int dstp = StringUtils::fromString<int>(fwe.dpt);

    bool keyIsSrc = false;
    if ( dstp < 1024 ) // srcport is default key
    {  
        sIter = _svcMap.find(dstp);
        fwe.protom.append("/").append(fwe.dpt);
    } else {
        sIter = _svcMap.find(srcp);
        fwe.protom.append("/").append(fwe.spt);
        keyIsSrc = true;
    }

    if ( sIter != _svcMap.end() )
    {
        FwService & svc = sIter->second;
        fwe.protom.append("(").append(svc.name).append("\\").append(fwe.proto).append(")");
    }

    fIter = _fwMap.find(fwe.absname);

    if ( fIter == _fwMap.end() ) {
        FwLogEntry::SetHostname(fwe);
        std::pair<FwMap::iterator, bool> insertR;
        insertR = _fwMap.insert( make_pair(fwe.absname, fwe) );

        if ( insertR.second ) {
            fIter = insertR.first;
            _api->add(fwe.absname, now);
            _api->add(fwe.absname + "/LastSeen", now);
            _api->add(fwe.absname + "/Hits", now);

            _api->add(fwe.absname + "/Src_hostname", now);
            if ( ! fwe.host.empty() ) {
                _api->update(fwe.absname + "/Src_hostname", now, fwe.host);
            }

            _api->add(fwe.protom, now);
            _api->add(fwe.protom + "/LastSeen", now);
            if ( keyIsSrc ) {
                _api->add(fwe.protom + "/SrcPort", now);
                _api->add(fwe.protom + "/" + fwe.dpt + "/DstPort", now);
                _api->add(fwe.protom + "/" + fwe.dpt + "/Protocol", now);
                _api->update(fwe.protom + "/" + fwe.dpt + "/Protocol", now, fwe.proto);
            } else {
                _api->add(fwe.protom + "/DstPort", now);
                _api->add(fwe.protom + "/" + fwe.spt + "/SrcPort", now);
                _api->add(fwe.protom + "/" + fwe.dpt + "/Protocol", now);
                _api->update(fwe.protom + "/" + fwe.dpt + "/Protocol", now, fwe.proto);
            }
        } else {
            return;
        }
    }

    LogFacility::LogMessage("FwLogReport: " + fwe.protom);

    FwLogEntry & entry = fIter->second;
    entry.count++;

    _api->update(fwe.absname, now, entry.count, TNMS_UINT64);
    _api->update(fwe.absname + "/LastSeen", now, entry.date);
    _api->update(fwe.absname + "/Hits", now, entry.count, TNMS_UINT64);
    
    _api->update(fwe.protom, now, entry.count, TNMS_UINT64);
    _api->update(fwe.protom + "/LastSeen", now, entry.date);

    return;
}


} // namespace


