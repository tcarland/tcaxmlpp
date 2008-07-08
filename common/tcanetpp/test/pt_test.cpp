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
                  "192.168.0.0/16"
                };

char* addrs2[] = { "10.0.0.0/8", "192.168.0.0/16" };



void printNodeHandler(ipv4addr_t addr, void* rock)
{
    printf("node address is %s,  ", CidrUtils::ntop(addr).c_str());
    if ( rock ) printf("rock is valid\n");
    return;
}

void debugNodeHandler ( ptNode_t* node )
{
    int i;
    printf("node addr: %s, node bit: %d\n",
        CidrUtils::ntop(node->key).c_str(), node->bit);

    if ( node->rlink )
        printf("right link addr is %s  ", 
            CidrUtils::ntop( ((node->rlink)->key) ).c_str());

    if ( node->llink )
        printf("left link addr is %s\n",
            CidrUtils::ntop( ((node->llink)->key) ).c_str());

    if ( (node->flags & PT_DELETE_FLAG) > 0 )
        printf("delete flag set\n");

    for ( i = 0; i < MAX_MASKLEN; i++ ) {
        if ( (node->rocks[i]) )
            printf("Rock found for entry %d\n", i);
    }

    return;
}

void nodeFreeHandler ( uint32_t addr, uint8_t mb, void* rock )
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

    std::vector<Prefix> srcp;
    std::vector<Prefix>::iterator vIter;

    for ( int i = 0; i < 5; i++ ) {
        Prefix p = CidrUtils::StringToCidr(addrs[i]);
        srcp.push_back(p);
        CidrUtils::deAggregate(p, 24, srcp);
    }

    printf("v size is %d\n", srcp.size());

    //for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ )
        //printf("Prefix is %s\n", CidrUtils::toCidrString(*vIter).c_str());
    //return 0;

    ptree = pt_init();

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ )
        pt_insert(ptree, vIter->getCidr(), (void*)(new Prefix(*vIter)));

    
    printf("Finished insert, nodecnt is %d\n", pt_nodes(ptree));
    pt_visit_node(ptree, &debugNodeHandler);
    sleep(1);

/*
    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
        if ( (tmp = pt_match(ptree, vIter->getCidr())) == 0 )
            printf("Search failed for %s\n", 
                CidrUtils::toString(vIter->getPrefix()).c_str());
        else
            printf("Found addr %s\n",
                CidrUtils::toString(vIter->getPrefix()).c_str());
    }
    sleep(1);
 */

    Prefix p      = CidrUtils::StringToCidr(std::string(addrs[1]));
    Prefix *pptr  = (Prefix*) pt_remove(ptree, p.getCidr());

    if ( pptr ) {
        printf("removed address %s/%d\n", CidrUtils::ntop(pptr->getPrefix()).c_str(),
            pptr->getPrefixLen());
        delete pptr;
    }
   
    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
        if ( (tmp = pt_match(ptree, vIter->getCidr())) == 0 )
            printf("Search failed for %s\n", 
                CidrUtils::toString(vIter->getPrefix()).c_str());
        else
            printf("Found addr %s\n",
                CidrUtils::toString(vIter->getPrefix()).c_str());
    }


/*

    tmp = pt_match(ptree, addr1);

    if ( tmp == 0 )
        printf("Search failed for %s\n", CidrUtils::toString(addr1).c_str());
    else 
        printf("Found addr %s\n", CidrUtils::toString(addr1).c_str() );


    tmp = pt_match(ptree, addr2);

    if ( tmp == 0 )
        printf("Search failed for %s\n",  CidrUtils::toString(addr2).c_str());
    else {
        printf("Found addr %s\n",  CidrUtils::toString(addr2).c_str());
        //pt_remove(ptree, addr2);
    }

    tmp = pt_match(ptree, addr2);

    if ( tmp == 0 ) {
        printf("Search failed for %s\n",  CidrUtils::toString(addr2).c_str());
        pt_insert(ptree, addr2, NULL);
    } else {
        printf("Found addr %s\n",  CidrUtils::toString(addr2).c_str());
    }


    tmp = pt_match(ptree, addr3);

    if ( tmp == 0 )
        printf("Search failed for %s\n",  CidrUtils::toString(addr3).c_str());
    else {
        printf("Found addr %s\n",  CidrUtils::toString(addr3).c_str());
    }

    tmp = pt_match(ptree, addr4);

    if ( tmp == 0 )
        printf("Search failed for %s\n",  CidrUtils::toString(addr4).c_str());
    else {
        printf("Found addr %s\n",  CidrUtils::toString(addr4).c_str());
    }

    tmp = pt_match(ptree, addr5);

    if ( tmp == 0 )
        printf("Search failed for %s\n",  CidrUtils::toString(addr5).c_str());
    else {
        printf("Found addr %s\n",  CidrUtils::toString(addr5).c_str());
    }

    //pt_visit(ptree, &printNodeHandler);

    //pt_visit_debug(ptree, &debugNodeHandler);
*/


    pt_free(ptree, &nodeFreeHandler);

    return 0;
}
