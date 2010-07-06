
#include <iostream>

#include "FwLogReport.h"


namespace fwgen {



FwLogReport::FwLogReport ( const std::string & agent, const std::string & host, uint16_t port )
    : _connection(false)
{
    _api = new TnmsAPI(agent, host, port);
    //_api->set_config("etc/fwlr_comet.xml");

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
        std::cout << "FwLogReport::FlushApi() no API connection, attempting reconnect"
            << std::endl;

    do {
        retval = _api->send(now);
        
        if ( retval == 1 ) {
            std::cout << "  FwLogReport::FlushApi(): API has Invalid configuration" << std::endl;
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
                std::cout << "  FwLogReport::FlushApi() Not authorized." << std::endl;
                _connection = false;
                break;
            }
            errcnt++;
        } else {
            if ( ! _connection )
                std::cout << std::endl << "  FwLogReport::FlushApi(): Connected." << std::endl;
            _connection = true;
        }
    } while ( retval > 0 && errcnt < 8 );

    if ( ! _connection )
        std::cout << std::endl << "   FwLogReport::FlushApi(): Connection failed" << std::endl;

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
    fwe.absname.append("<=>").append(fwe.dst);
    fwe.protom  = fwe.absname;

    int srcp = StringUtils::fromString<int>(fwe.spt);
    int dstp = StringUtils::fromString<int>(fwe.dpt);

    bool keyIsSrc = false;
    if ( dstp < 1024 )
    {  // srcport is key
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

            if ( ! fwe.host.empty() ) {
                _api->add(fwe.absname + "/Src_hostname", now);
                _api->update(fwe.absname + "/Src_hostname", now, fwe.host);
            }

            _api->add(fwe.protom, now);
            _api->add(fwe.protom + "/LastSeen", now);
            if ( keyIsSrc )
                _api->update(fwe.protom, "SrcPort");
            else
                _api->update(fwe.protom, "DstPort");

        } else {
            return;
        }
    }

    std::cout << "FwLogReport entry: '" << fwe.protom << "'" << std::endl;

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


