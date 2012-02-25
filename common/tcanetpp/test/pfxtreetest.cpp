
#include "Socket.h"
#include "PrefixTree.hpp"
using namespace tcanetpp;


const char* addr1 = "10.0.0.0/8";
const char* addr2 = "10.10.0.0/16";
const char* addr3 = "10.10.10.0/24";
const char* addr4 = "172.16.0.0";
const char* addr5 = "172.16.8.0";


const  char* addrs[] = { "10.0.0.0/8",
                         "10.10.0.0/16",
  	                 "12.0.0.0/16",
                         "172.16.0.0/16",
                         "192.168.0.0/16",
                         "172.16.8.0/24"
                       };

const char * addrs2[] = { "10.0.0.0/8", "192.168.0.0/16" };

static
void printNodeHandler ( ipv4addr_t addr, void * rock )
{
    printf("node address is %s,  ", IpAddr::ntop(addr).c_str());
    if ( rock ) printf("rock is valid\n");
    return;
}

static
void debugNodeHandler ( ptNode_t * node )
{
    int i;
    printf("node addr: %s, node bit: %d\n",
	IpAddr::ntop(node->key).c_str(), node->bit);

    if ( node->rlink )
	printf("right link addr is %s  ", 
	    IpAddr::ntop( ((node->rlink)->key) ).c_str());

    if ( node->llink )
	printf("left link addr is %s\n",
	    IpAddr::ntop( ((node->llink)->key) ).c_str());

    if ( (node->flags & PT_DELETE_FLAG) > 0 )
	printf("delete flag set\n");

    for ( i = 0; i < MAXMASKLEN; i++ ) {
	if ( (node->rocks[i]) )
	    printf("Rock found for entry %d\n", i);
    }

    return;
}

static
void nodeFreeHandler ( uint32_t addr, uint16_t mb, void * rock )
{
    IpAddr * p = (IpAddr*) rock;
    if ( p ) {
	printf("deleting address %s/%d\n", IpAddr::ntop(p->getPrefix()).c_str(),
	    p->getPrefixLen());
	delete p;
    }
}


int main ( int argc, char **argv )
{
    PrefixTree<IpAddr*>  ptree;

    ptree.setFreeHandler(&nodeFreeHandler);

    std::vector<IpAddr> srcp;
    std::vector<IpAddr>::iterator vIter;

    for ( int i = 0; i < 6; i++ ) {
	IpAddr p;
	IpAddr::ToIpAddr(addrs[i], p);
	srcp.push_back(p);
	//IpAddr::deAggregate(p, 24, srcp);
    }
    
    printf("v size is %lu\n", srcp.size());

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ )
	ptree.insert(*vIter, (new IpAddr(*vIter)));

    sleep(1);

    printf("patricia size is %d nodecnt is %d\n", ptree.size(), ptree.nodes());
   
    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
	IpAddr * p  = ptree.exactMatch(*vIter);
	if ( p == NULL )
	    printf("Search failed for %s\n",  vIter->toString().c_str());
	else
	    printf("Found addr %s\n", vIter->toString().c_str());
    }

    IpAddr psp;
    IpAddr * p  = NULL;

    IpAddr::ToIpAddr(addr3, psp);
    p = ptree.longestMatch(psp);
    if ( p == NULL )
	printf("Search failed for %s\n", psp.toString().c_str());
    else
	printf("longest match for %s returned %s\n",
	       psp.toString().c_str(), p->toString().c_str());

    return 0;
}

