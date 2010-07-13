#define _pfxcachetest_cpp_

#include <iostream>
#include <sys/time.h>

#include "PrefixCache.hpp"
#include "CidrUtils.h"
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
    Prefix   p;
    myitem * item = NULL;
    time_t   now  = 0;

    PrefixCache<myitem*>  pc = PrefixCache<myitem*>(30, false);

    pc.cacheTimeout(5);
    now = ::time(NULL);

    for ( int i = 0; i < 6; i++ )
    {
        now += 5;
        if ( CidrUtils::StringToCidr(addrs[i], p) == 0 ) {
            std::cout << "error in address string " << std::endl;
        } else {
            item = new myitem(now);
            item->addrstr = CidrUtils::ToString(p);
            pc.insert(p, item, now);
	    std::cout << "Inserted prefix = " << CidrUtils::ToString(p) << std::endl;
        }
    }

    std::cout << " pc size = " << pc.size() << std::endl;



    time_t  runtime = 60;
    time_t  start   = 0;

    now    = ::time(NULL);
    start  = now;

    while ( now < (start + runtime) )
    {
        std::list<myitem*>            stales;
        std::list<myitem*>::iterator  sIter;

        if ( pc.size() == 0 )
            break;

        pc.expireStale(now, stales);

        if ( stales.size() > 0 )
            std::cout << "Cache removed " << stales.size() << " items at " << ctime(&now);

        for ( sIter = stales.begin(); sIter != stales.end(); ++sIter ) {
            item = *sIter;
            std::cout << "  removed " << item->addrstr << std::endl;
            if ( item )
                delete item;
        }

        sleep(2);

        now = ::time(NULL);
    }

    std::cout << "finished." << std::endl;

    return 0;
}

