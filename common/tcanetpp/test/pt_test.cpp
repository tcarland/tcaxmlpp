#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

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



void printNodeHandler ( ipv4addr_t addr, void * rock )
{
    printf("node address is %s,  ", IpAddr::ntop(addr).c_str());
    if ( rock ) printf("rock is valid\n");
    return;
}

void debugNodeHandler ( ptNode_t * node )
{
    int i;
    
    std::string  ip  = IpAddr::ntop(node->key);
 
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

void nodeFreeHandler ( uint32_t addr, uint16_t mb, void * rock )
{
    IpAddr * p = (IpAddr*) rock;
    if ( p ) {
        printf("deleting address %s\n", p->toString().c_str());
        delete p;
    }
}


int main ( int argc, char **argv )
{
    ptNode_t*   ptree;
    int         tmp;
    IpAddr      pfx;

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

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ )
        pt_insert(ptree, vIter->getCidr(), (void*)(new IpAddr(*vIter)));

    
    printf("Finished insert, nodecnt is %d\n", pt_nodes(ptree));
    pt_visit_node(ptree, &debugNodeHandler);
    sleep(1);


    IpAddr  p;
    IpAddr::ToIpAddr(addrs[1], p);
    IpAddr * pptr = (IpAddr*) pt_remove(ptree, p.getCidr());

    if ( pptr ) {
        printf("removed address %s\n", pptr->toString().c_str());
        delete pptr;
    }
   
    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
        if ( (tmp = pt_exists(ptree, vIter->getCidr())) == 0 )
            printf("Search failed for %s\n", vIter->toString().c_str());
        else
            printf("Found addr %s\n", vIter->toString().c_str());
    }

    pt_free(ptree, &nodeFreeHandler);

    return 0;
}
