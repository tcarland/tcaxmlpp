
#include <iostream>

#include "FwLogReport.h"


namespace fwgen {



FwLogReport::FwLogReport ( const std::string & agent )
    : _connection(false)
{
    _api = new TnmsAPI(agent, "localhost", 15300);
    _api->set_config("etc/fwlr_comet.xml");

    FwService::ParseServices(ETC_SERVICES, _svcMap);
}


int
FwLogReport::FlushApi ( const time_t & now )
{
    int  retval = 0;
    int  errcnt = 0;

    if ( ! _connection )
        std::cout << "FwLogReport: no API connection, attempting reconnect" << std::endl;

    do {
        retval = _api->send(now);
        
        if ( retval == 1 ) {
            std::cout << "Invalid configuration" << std::endl;
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
                std::cout << "Not authorized." << std::endl;
                _connection = false;
                break;
            }
            errcnt++;
        } else {
            if ( ! _connection )
                std::cout << std::endl << "Connected." << std::endl;
            _connection = true;
        }
    } while ( retval > 0 && errcnt < 8 );

    if ( ! _connection )
        std::cout << std::endl << "  Connection failed" << std::endl;

    //std::cout << std::endl;

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

            if ( ! fwe.host.empty() ) {
                _api->add(fwe.absname + "/Src_hostname", now);
                _api->update(fwe.absname + "/Src_hostname", now, fwe.host);
            }

            _api->add(fwe.protom, now);
            if ( keyIsSrc )
                _api->update(fwe.protom, "SrcPort");
            else
                _api->update(fwe.protom, "DstPort");
            _api->add(fwe.protom + "/LastSeen", now);
        } else {
            return;
        }
    }

    std::cout << "FwLogReport entry: '" << fwe.protom << "'" << std::endl;

    FwLogEntry & entry = fIter->second;
    entry.count++;

    _api->update(fwe.absname, now, entry.count, TNMS_UINT64);
    _api->update(fwe.absname + "/LastSeen", now, entry.date);
    
    _api->update(fwe.protom, now, entry.count, TNMS_UINT64);
    _api->update(fwe.protom + "/LastSeen", now, entry.date);

    return;
}


} // namespace


