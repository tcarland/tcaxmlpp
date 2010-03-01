#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include <vector>

#include "patricia.h"

#include "CidrUtils.h"

using namespace tcanetpp;


char* addrstr1 = "10.0.0.0/8";
char* addrstr2 = "10.10.0.0/16";
char* addrstr3 = "10.10.10.0/24";
char* addrstr4 = "172.16.0.0";
char* addrstr5 = "172.16.8.0";


char* addrs[] = { "10.0.0.0/8",
                  "10.10.10.0/24",
                  "12.0.0.0/16",
                  "172.16.0.0/16",
                  "192.168.0.0/16",
                  "192.202.0.0/20"
                };

char* addrs2[] = { "10.0.0.0/8", "192.168.0.0/16" };



void printNodeHandler(ipv4addr_t addr, void * rock)
{
    printf("node address is %s,  ", CidrUtils::ntop(addr).c_str());
    if ( rock ) printf("rock is valid\n");
    return;
}

void debugNodeHandler ( ptNode_t * node )
{
    int i;
    
    std::string  ip  = CidrUtils::ntop(node->key);
 
    printf("debugNodeHandler() node addr: %s, node bit: %d\n",
        ip.c_str(), node->bit);
/*
    if ( node->rlink )
        printf("  right link addr is %s  ", 
            CidrUtils::ntop( ((node->rlink)->key) ).c_str());

    if ( node->llink )
        printf("  left link addr is %s\n",
            CidrUtils::ntop( ((node->llink)->key) ).c_str());
*/
    if ( (node->flags & PT_DELETE_FLAG) > 0 )
        printf("  delete flag set\n");

    for ( i = 0; i < PT_MASKLEN; i++ ) {
        if ( (node->rocks[i]) )
            printf("  entry found for %s/%d\n", ip.c_str(), i);
    }

    return;
}

void nodeFreeHandler ( uint32_t addr, uint8_t mb, void * rock )
{
    Prefix* p = (Prefix*) rock;
    if ( p ) {
        printf("deleting address %s/%d\n", CidrUtils::ntop(p->getPrefix()).c_str(),
            p->getPrefixLen());
        delete p;
    }
}


int main ( int argc, char **argv )
{
    ptNode_t*   ptree;
    int         tmp;
    Prefix      pfx;

    std::vector<Prefix> srcp;
    std::vector<Prefix>::iterator vIter;

    for ( int i = 0; i < 6; i++ ) {
        CidrUtils::StringToCidr(addrs[i], pfx);
        srcp.push_back(pfx);
    }

    CidrUtils::StringToCidr(addrs[5], pfx);
    CidrUtils::DeAggregate(pfx, 24, srcp);
    

    printf("v size is %lu\n", srcp.size());

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ )
        printf("Prefix is %s\n", CidrUtils::ToString(*vIter).c_str());
    //return 0;

    ptree = pt_init();

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ )
        pt_insert(ptree, vIter->getCidr(), (void*)(new Prefix(*vIter)));

    
    printf("Finished insert, nodecnt is %d\n", pt_nodes(ptree));
    pt_visit_node(ptree, &debugNodeHandler);
    sleep(1);


    Prefix  p;
    CidrUtils::StringToCidr(addrs[1], p);
    Prefix *pptr  = (Prefix*) pt_remove(ptree, p.getCidr());

    if ( pptr ) {
        printf("removed address %s/%d\n", 
                CidrUtils::ntop(pptr->getPrefix()).c_str(),
                pptr->getPrefixLen());
        delete pptr;
    }
   
    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
        if ( (tmp = pt_exists(ptree, vIter->getCidr())) == 0 )
            printf("Search failed for %s\n", 
                CidrUtils::ToString(vIter->getPrefix()).c_str());
        else
            printf("Found addr %s\n",
                CidrUtils::ToString(vIter->getPrefix()).c_str());
    }

    pt_free(ptree, &nodeFreeHandler);

    return 0;
}
