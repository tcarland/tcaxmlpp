#define _pfxcachetest_cpp_

#include <iostream>
#include <sys/time.h>

#include "PrefixCache.hpp"
#include "IpAddr.h"
using namespace tcanetpp;


const char * addr1 = "10.0.0.0/8";
const char * addr2 = "10.10.0.0/16";
const char * addr3 = "10.10.10.0/24";
const char * addr4 = "172.16.0.0";
const char * addr5 = "172.16.8.0";

const
char * addrs[] = { "10.0.0.0/8",
                   "10.10.0.0/16",
                   "12.0.0.0/16",
                   "172.16.0.0/16",
                   "192.168.0.0/16",
                   "172.16.8.0/24"
                 };


struct myitem {
    std::string  addrstr;
    time_t       t;

    myitem() : t(0) {}
    myitem ( time_t now ) : t(now) {}
};


int main ( int argc, char **argv )
{
    IpAddr   p;
    myitem * item = NULL;
    time_t   now  = 0;

    PrefixCache<myitem*>  pc = PrefixCache<myitem*>(10, false);

    now = ::time(NULL);

    for ( int i = 0; i < 6; i++ )
    {
        now += 5;
        if ( IpAddr::ToIpAddr(addrs[i], p) == 0 ) {
            std::cout << "Error in address string " << addrs[i] << std::endl;
        } else {
            item = new myitem(now);
            item->addrstr = p.toString();
            if ( ! pc.insert(p, item, now) )
                std::cout << "Error inserting prefix " << p.toString();
            else
	        std::cout << "Inserted prefix = " << IpAddr::ToPrefixStr(p);
            std::cout << std::endl;
        }
    }

    std::cout << " PrefixCache size = " << pc.size() << std::endl;

    // ---------- //

    time_t  runtime = 60;
    time_t  start   = 0;

    now    = ::time(NULL);
    start  = now;

    std::cout << " Cache timeout set to " << pc.cacheTimeout() << std::endl;
    std::cout << "Starting timed loop at: (" << now << ")  " 
        << ctime(&now) << std::endl;


    while ( now < (start + runtime) )
    {
        std::list<myitem*>            stales;
        std::list<myitem*>::iterator  sIter;

        if ( pc.size() == 0 )
            break;

        pc.expireStale(now, stales);

        if ( stales.size() > 0 )
            std::cout << "Cache expired " << stales.size() << " item(s) at: ("
                << now << ")  " << ctime(&now);

        for ( sIter = stales.begin(); sIter != stales.end(); ++sIter ) {
            item = *sIter;
            std::cout << "  Removed: " << item->addrstr << std::endl;
            if ( item )
                delete item;
        }

        sleep(1);

        now = ::time(NULL);
    }

    std::cout << std::endl << "Finished:  (" << now << ")  " << ctime(&now);

    return 0;
}

