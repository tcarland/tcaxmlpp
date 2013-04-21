#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <vector>

#include "patricia.h"

#include "IpAddr.h"

using namespace tcanetpp;


const char* addrstr1 = "10.0.0.0/8";
const char* addrstr2 = "10.10.0.0/16";
const char* addrstr3 = "10.10.10.0/24";
const char* addrstr4 = "172.16.0.0";
const char* addrstr5 = "172.16.8.0";


const char* addrs[] = { "10.0.0.0/8",
                        "10.10.10.0/24",
                        "12.0.0.0/16",
                        "172.16.0.0/16",
                        "192.168.0.0/16",
                        "192.202.0.0/20"
                      };

const char* addrs2[] = { "10.0.0.0/8", "192.168.0.0/16" };

const char* addrs6[] = { "fd00:0a0a:7fd0::/48",
                         "fd00:0a0a:7fd0:a000::/52",
                         "fd00:0a0a:7fd0:a000::/56",
                         "fd00:0a0a:7fd0:a100::/56",
                         "fd00:0a0a:7fd0:a200::/56",
                         "fd00:0a0a:7fd0:aa0a:021b:21ff:fe2f:f99c/64"
                       };

const char* addrstr6 = "fd00:0a0a:7fd0:a200:021b:21ff:fe2d::/64";


void printNodeHandler ( uint64_t addrA, uint64_t addrB, uint16_t mb, void * rock )
{
    std::string ip;

    if ( addrA == addrB )
        ip = IpAddr::ntop(addrB);
    else
        ip = IpAddr::ntop(ipv6addr_t(addrA, addrB));

    printf("node address is %s/%d\n", ip.c_str(), mb);
    if ( rock ) printf("  rock is valid\n");
    return;
}

void debugNodeHandler ( ptNode_t * node )
{
    std::string  ip;
    int  i;

    if ( pt_is_ipv4(node) )
        ip = IpAddr::ntop(pt_to_ipv4(node));
    else
        ip = IpAddr::ntop(ipv6addr_t(node->key, node->host));

    printf("debugNodeHandler() node addr: %s, node bit: %d\n",
        ip.c_str(), node->bit);
/*
    if ( node->rlink )
        printf("  right link addr is %s  ", 
            IpAddr::ntop( ((node->rlink)->key) ).c_str());

    if ( node->llink )
        printf("  left link addr is %s\n",
            IpAddr::ntop( ((node->llink)->key) ).c_str());
*/
    if ( (node->flags & PT_DELETE_FLAG) > 0 )
        printf("  delete flag set\n");

    for ( i = 0; i < PT_MASKLEN; i++ ) {
        if ( (node->rocks[i]) )
            printf("  entry found for %s/%d\n", ip.c_str(), i);
    }

    return;
}

void nodeFreeHandler ( uint64_t addrA, uint64_t addrB, uint16_t mb, void * rock )
{
    IpAddr * p = (IpAddr*) rock;
    if ( p ) {
        printf("deleting address %s\n", IpAddr::ToPrefixStr(*p).c_str());
        delete p;
    }
}


int main ( int argc, char **argv )
{
    ptNode_t*   ptree;
    int         tmp;
    IpAddr      pfx;
    cidr_t      c;

    IpAddrList  srcp;
    IpAddrList::iterator  vIter;

    for ( int i = 0; i < 6; i++ ) {
        IpAddr::ToIpAddr(addrs[i], pfx);
        srcp.push_back(pfx);
    }

    IpAddr::ToIpAddr(addrs[5], pfx);
    IpAddr::DeAggregate(pfx, 24, srcp);

    printf("v size is %lu\n", srcp.size());

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ )
        printf("Prefix is %s\n", IpAddr::ToPrefixStr(*vIter).c_str());

    ptree = pt_init();

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
        c = vIter->getCidr();
        pt_insert(ptree, &c, (void*)(new IpAddr(*vIter)));
    }

    
    printf("Finished insert, nodecnt is %d\n", pt_nodes(ptree));
    pt_visit_node(ptree, &debugNodeHandler);
    sleep(1);


    IpAddr  p;
    IpAddr::ToIpAddr(addrs[1], p);
    c = p.getCidr();
    IpAddr * pptr = (IpAddr*) pt_remove(ptree, &c);

    if ( pptr ) {
        printf("removed address %s\n", pptr->toString().c_str());
        delete pptr;
    }

    // matchLongest
    pptr = (IpAddr*) pt_matchLongest(ptree, &c);
    if ( pptr )
        printf("MatchLongest found %s\n\n", pptr->toString().c_str());
   
    // exists
    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
        c = vIter->getCidr();
        if ( (tmp = pt_exists(ptree, &c)) == 0 )
            printf("Search failed for %s\n", vIter->toString().c_str());
        else
            printf("Found addr %s\n", vIter->toString().c_str());
    }

    printf("pt size is %d\n", pt_size(ptree));

    srcp.clear();

    ipv6addr_t addr6;
    for ( int i = 0; i < 6; i++ ) {
        if ( IpAddr::ToIpAddr(addrs6[i], pfx) == 0 )
            printf("Error in pton for %s\n", addrs6[i]);
        srcp.push_back(pfx);
    }

    printf("v size is %lu\n\n", srcp.size());

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
        printf("Prefix is %s\n", IpAddr::ToPrefixStr(*vIter).c_str());
        c = vIter->getCidr();
        pt_insert(ptree, &c, (void*)(new IpAddr(*vIter)));
    }

    IpAddr::ToIpAddr(addrstr6, p);
    c = p.getCidr();
    // matchLongest
    pptr = (IpAddr*) pt_matchLongest(ptree, &c);
    if ( pptr )
        printf("MatchLongest found %s\n", IpAddr::ToPrefixStr(*pptr).c_str());
    else
        printf("ERROR: Match not found\n");

    printf("Finished insert, nodecnt is %d\n", pt_nodes(ptree));

    pt_visit_node(ptree, &debugNodeHandler);
    pt_free(ptree, &nodeFreeHandler);

    printf("pt size is %d\n", pt_size(ptree));

    return 0;
}
