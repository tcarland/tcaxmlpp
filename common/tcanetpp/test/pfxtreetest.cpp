
#include "Socket.h"
#include "CidrUtils.h"
#include "PrefixTree.hpp"
using namespace tcanetpp;


char * addr1 = "10.0.0.0/8";
char * addr2 = "10.10.0.0/16";
char * addr3 = "10.10.10.0/24";
char * addr4 = "172.16.0.0";
char * addr5 = "172.16.8.0";


char * addrs[] = { "10.0.0.0/8",
                   "10.10.0.0/16",
  	           "12.0.0.0/16",
                   "172.16.0.0/16",
		   "192.168.0.0/16",
		   "172.16.8.0/24"
		 };

char * addrs2[] = { "10.0.0.0/8", "192.168.0.0/16" };


void printNodeHandler ( ipv4addr_t addr, void * rock )
{
    printf("node address is %s,  ", CidrUtils::ntop(addr).c_str());
    if ( rock ) printf("rock is valid\n");
    return;
}

void debugNodeHandler ( ptNode_t * node )
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

void nodeFreeHandler ( uint32_t addr, uint8_t mb, void * rock )
{
    Prefix * p = (Prefix*) rock;
    if ( p ) {
	printf("deleting address %s/%d\n", CidrUtils::ntop(p->getPrefix()).c_str(),
	    p->getPrefixLen());
	delete p;
    }
}


int main ( int argc, char **argv )
{
    PrefixTree<Prefix*>  ptree;

    ptree.setFreeHandler(&nodeFreeHandler);

    std::vector<Prefix> srcp;
    std::vector<Prefix>::iterator vIter;

    for ( int i = 0; i < 6; i++ ) {
	Prefix p = CidrUtils::StringToCidr(addrs[i]);
	srcp.push_back(p);
	//CidrUtils::deAggregate(p, 24, srcp);
    }
    
    printf("v size is %d\n", srcp.size());

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ )
	ptree.insert(*vIter, (new Prefix(*vIter)));

    sleep(1);

    printf("patricia size is %d nodecnt is %d\n", ptree.size(), ptree.nodes());
   
    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
	Prefix * p = NULL;
	p = ptree.exactMatch(*vIter);
	if ( p == NULL )
	    printf("Search failed for %s\n", 
		CidrUtils::toString(vIter->getPrefix()).c_str());
	else
	    printf("Found addr %s\n",
		CidrUtils::toString(vIter->getPrefix()).c_str());
    }

    Prefix psp  = CidrUtils::StringToCidr(addr3);
    Prefix * p  = NULL;

    p = ptree.longestMatch(psp);
    if ( p == NULL )
	printf("Search failed for %s\n", 
	    CidrUtils::toCidrString(psp).c_str());
    else
	printf("longest match for %s returned %s\n",
	    CidrUtils::toCidrString(psp).c_str(),
	    CidrUtils::toCidrString(*p).c_str());

    return 0;
}

