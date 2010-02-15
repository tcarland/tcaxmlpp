
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
                std::cout << std::endl << "Connected.";
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
    FwMap::iterator  fIter;
    std::string   absname = fwe.inf;

    if ( fwe.inf.empty() )
        return;

    absname.append("/").append(fwe.src);
    absname.append("<=>").append(fwe.dst);
    absname.append(":").append(fwe.dpt);
    absname.append("/").append(fwe.proto);

    fIter = _fwMap.find(absname);

    if ( fIter == _fwMap.end() ) {
        std::pair<FwMap::iterator, bool> insertR;
        insertR = _fwMap.insert( make_pair(absname, fwe) );
        if ( insertR.second ) {
            fIter = insertR.first;
            _api->add(absname, now);
        } else {
            return;
        }
    }

    FwLogEntry & entry = fIter->second;
    entry.count++;

    _api->update(absname, now, entry.count, TNMS_UINT64);

    return;
}


} // namespace


