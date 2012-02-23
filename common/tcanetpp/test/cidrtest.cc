
#include <string.h>
#include <cstdio>
#include <iostream>

#include "CidrUtils.h"

using namespace tcanetpp;


int main ( int argc, char **argv )
{

    ipv6addr_t    addr;
    std::string   result, name;
    int           r, i;

    addr = in6addr_loopback;
    r    = CidrUtils::GetNameInfo(addr, result, NI_NUMERICHOST);

    std::cout << "ipv6 loopback: r = " << r << "  nameinfo: " 
              << result << std::endl;


    name  = CidrUtils::GetHostName();
    printf("Localhost name: %s\n\n", name.c_str());

    // Query addrs for local host
    struct addrinfo   hints, *res, *ai;

    name  = "comet.ratnest.org";
    hints = CidrUtils::GetTCPServerHints();
    r     = CidrUtils::GetAddrInfo(name, &hints, &res);
    ai    = res;
    i     = 1;

    std::cout << "  " << name << std::endl;
    while ( ai ) {
        CidrUtils::GetNameInfo((const sockaddr*) ai->ai_addr, ai->ai_addrlen, result, NI_NUMERICHOST);
        std::cout << "addr result: " << i << " > " << result << std::endl;
        i++;
        ai = ai->ai_next;
    }

    // Query for all host addrs for a given forward
    name  = "www.google.com";

    hints = CidrUtils::GetTCPClientHints();
    r     = CidrUtils::GetAddrInfo(name, &hints, &res);
    ai    = res;
    i     = 1;

    std::cout << "  " << name << std::endl;
    while ( ai ) {
        CidrUtils::GetNameInfo((const sockaddr*) ai->ai_addr, ai->ai_addrlen, result, NI_NUMERICHOST);
        std::cout << "addr result: " << i << " > " << result << std::endl;
        i++;
        ai = ai->ai_next;
    }

    if ( argc > 1 )
        name.assign(argv[1]);

    IpAddrList  alist;
    IpAddrList::iterator  aIter;

    CidrUtils::GetHostAddrList(name, alist);
    int c = 1;

    printf("Resolving addresses for %s\n", name.c_str());

    for ( aIter = alist.begin(); aIter != alist.end(); ++aIter, ++c ) 
    {
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
    
