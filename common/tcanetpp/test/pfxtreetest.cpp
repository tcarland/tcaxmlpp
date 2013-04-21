
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

const char* addrs6[] = { "fd00:0a0a:7fd0::/48",
                         "fd00:0a0a:7fd0:a000::/52",
                         "fd00:0a0a:7fd0:a000::/56",
                         "fd00:0a0a:7fd0:a100::/56",
                         "fd00:0a0a:7fd0:a200::/56",
                         "fd00:0a0a:7fd0:aa0a:021b:21ff:fe2f:f99c/64"
                       };

const char* addrstr6 = "fd00:0a0a:7fd0:a200:021b:21ff:fe2d::/64";


static
void printNodeHandler ( uint64_t addrA, uint64_t addrB, uint16_t mb, void * rock )
{
    std::string ip;
    if ( addrA == addrB )
        ip = IpAddr::ntop(addrB);
    else
        ip = IpAddr::ntop(ipv6addr_t(addrA, addrB));

    printf("node address is %s/%d\n", ip.c_str(), mb);

    if ( rock ) 
        printf("  rock is valid\n");

    return;
}

static
void debugNodeHandler ( ptNode_t * node )
{
    std::string ip;
    int i;

    if ( pt_is_ipv4(node) )
        ip = IpAddr::ntop(pt_to_ipv4(node));
    else
        ip = IpAddr::ntop(ipv6addr_t(node->key, node->host));

    printf("node addr: %s, node bit: %d\n", ip.c_str(), node->bit);

    if ( node->rlink )
	printf("right link addr is %s  ", 
	    IpAddr::ntop( ((node->rlink)->key) ).c_str());

    if ( node->llink )
	printf("left link addr is %s\n",
	    IpAddr::ntop( ((node->llink)->key) ).c_str());

    if ( (node->flags & PT_DELETE_FLAG) > 0 )
	printf("delete flag set\n");

    for ( i = 0; i < 64; i++ ) {
	if ( (node->rocks[i]) )
	    printf("Rock found for entry %d\n", i);
    }

    return;
}

static
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
    IpAddr   pfx;
    IpAddr * p;
    PrefixTree<IpAddr*>  ptree;

    ptree.setFreeHandler(&nodeFreeHandler);

    std::vector<IpAddr> srcp;
    std::vector<IpAddr>::iterator vIter;

    for ( int i = 0; i < 6; i++ ) {
	IpAddr::ToIpAddr(addrs[i], pfx);
	srcp.push_back(pfx);
	//IpAddr::deAggregate(p, 24, srcp);
    }

    for ( int i = 0; i < 6; i++ ) {
        IpAddr::ToIpAddr(addrs6[i], pfx);
        srcp.push_back(pfx);
    }
    
    printf("v size is %lu\n", srcp.size());

    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ )
	ptree.insert(*vIter, (new IpAddr(*vIter)));

    sleep(1);

    printf("patricia size is %d nodecnt is %d\n", ptree.size(), ptree.nodes());
   
    for ( vIter = srcp.begin(); vIter != srcp.end(); vIter++ ) {
	p  = ptree.exactMatch(*vIter);
	if ( p == NULL )
	    printf("Search failed for %s\n",  vIter->toString().c_str());
	else
	    printf("Found addr %s\n", vIter->toString().c_str());
    }

    p  = NULL;

    IpAddr::ToIpAddr(addr3, pfx);
    p = ptree.longestMatch(pfx);
    if ( p == NULL )
	printf("Search failed for %s\n", pfx.toString().c_str());
    else
	printf("longest match for %s returned %s\n",
	       pfx.toString().c_str(), IpAddr::ToPrefixStr(*p).c_str());

    IpAddr::ToIpAddr(addrstr6, pfx);
    p  = ptree.longestMatch(pfx);
    if ( p == NULL )
	printf("Search failed for %s\n", pfx.toString().c_str());
    else
	printf("longest match for %s returned %s\n",
	       pfx.toString().c_str(), IpAddr::ToPrefixStr(*p).c_str());


    return 0;
}

