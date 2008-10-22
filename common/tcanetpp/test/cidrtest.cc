
#include <cstdio>

#include "CidrUtils.h"

using namespace tcanetpp;


int main ( int argc, char **argv )
{
    std::string localname = CidrUtils::getHostName();
    printf("Localhost name: %s\n\n", localname.c_str());

    // Query for all host addrs for a given forward
    std::string name = "www.google.com";

    if ( argc > 1 )
        name.assign(argv[1]);

    AddrList  alist;
    AddrList::iterator  aIter;

    CidrUtils::getHostAddrList(name, alist);
    int c = 1;

    printf("Resolving addresses for %s\n", name.c_str());

    for ( aIter = alist.begin(); aIter != alist.end(); ++aIter, ++c ) {
        ipv4addr_t addr = *aIter;
        printf("Addr %d is %u  :  %s\n", c, addr, CidrUtils::ntop(addr).c_str());
    }

    std::string  cidr = "192.102.249.0/32";
    Prefix       p;

    CidrUtils::StringToCidr(cidr, p);
    
    printf("\nProcessing string %s ...\n", cidr.c_str());
    printf("Result is %u / %u\n", p.getPrefix(), p.getPrefixLen());

    return 0;
}
    
