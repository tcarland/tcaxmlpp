/**  Patricia Trie implementation from Sedgewick
  *
  *  @Author   tcarland@gmail.com
  *  @Version  1.1
 **/
#define PATRICIA_C

#include <stdlib.h>
#include <string.h>

#include "patricia.h"


static const
char rcsid[] = "$Id: patricia.c,v 1.9 2003/10/03 21:27:19 tca Exp $";



/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *  internal static methods for recursively operating on trie
 *
 */

static cidr_t searchCidr;
static cidr_t resultCidr;
static int    freecnt    = 0;
static int    nodecnt    = 0;
static int    ptsize     = 0;


//  Create a new node
static ptNode_t*
PT_new ( cidr_t cidr, int bit, ptNode_t* llink, ptNode_t* rlink, void* rock )
{
    ptNode_t * np = NULL;

    np = (ptNode_t*) malloc (sizeof(ptNode_t));

    if ( np == NULL )
	return np;

    np->key   = cidr.addr;
    np->bit   = bit;
    np->flags = 0;
    np->llink = llink;
    np->rlink = rlink;

    memset(np->rocks, 0, sizeof(np->rocks));

    if ( rock )
	np->rocks[cidr.addrlen] = rock;

    return np;
}


//  Visits all nodes and children of the trie.
static void
PT_visitR ( ptNode_t* node, int bit, nodeHandler_t handler )
{
    int i;

    if ( node->bit <= bit ) {
	for ( i = 0; i < MAX_MASKLEN; i++ ) {
	    if ( (node->rocks[i]) && handler ) {
		handler(node->key, i, node->rocks[i]);
	    }
	}
    } else {
	PT_visitR(node->llink, node->bit, handler);
	PT_visitR(node->rlink, node->bit, handler);
    }
    return;
}


//  Visits all nodes of the trie.
static void
PT_visitR_node ( ptNode_t * node, int bit, pvtNodeHandler_t handler )
{
    if ( node->bit <= bit ) {
	if ( handler ) 
	    handler(node);
    } else {
	PT_visitR_node(node->llink, node->bit, handler);
	PT_visitR_node(node->rlink, node->bit, handler);
    }
    return;
}


//  recursive search in the trie
static ptNode_t*
PT_searchR ( ptNode_t * node, ipv4addr_t key, int bit )
{
    if ( node->bit <= bit )
	return node;

    if ( GETBIT(key, node->bit) == 0 )
	return PT_searchR(node->llink, key, node->bit);
    
    return PT_searchR(node->rlink, key, node->bit);
}


//  recursive insert into the trie 
static ptNode_t*
PT_insertR ( ptNode_t * h, cidr_t cidr, int bit, ptNode_t * p, void * rock )
{
    ptNode_t * newNode = NULL;

    if ( (h->bit >= bit) || (h->bit <= p->bit) ) {
	newNode = PT_new(cidr, bit, 0, 0, rock);

	if ( GETBIT(cidr.addr, bit) ) {
	    newNode->llink = h;
	    newNode->rlink = newNode;
	} else {
	    newNode->llink = newNode;
	    newNode->rlink = h;
	}

	return newNode;
    }

    if ( GETBIT(cidr.addr, h->bit) == 0 )
	h->llink = PT_insertR(h->llink, cidr, bit, h, rock);
    else
	h->rlink = PT_insertR(h->rlink, cidr, bit, h, rock);

    return h;
}


//  recursive removal from the trie
static void*
PT_removeR ( ptNode_t * node, cidr_t cidr, int bit )
{
    void * rock = NULL;

    if ( node->bit <= bit ) {
	if ( (node->key == cidr.addr) && (node->rocks[cidr.addrlen]) ) {
	    rock = node->rocks[cidr.addrlen];
	    node->rocks[cidr.addrlen] = NULL;
	}
	return rock;
    }
    
    if ( GETBIT(cidr.addr, node->bit) == 0 )
	return PT_removeR(node->llink, cidr, node->bit);

    return PT_removeR(node->rlink, cidr, node->bit);
}


//  recursively frees all nodes of the trie, except the root node.
static void
PT_freeNodesR ( ptNode_t * head, ptNode_t * node, int bit, nodeHandler_t handler )
{
    int i;

    if ( node->bit <= bit )
	return;

    PT_freeNodesR(head, node->llink, node->bit, handler);
    PT_freeNodesR(head, node->rlink, node->bit, handler);

    if ( node != head ) {
	for ( i = 0; i < MAX_MASKLEN; i++ ) {
	    if ( node->rocks[i] && handler )
		handler(node->key, i, node->rocks[i]);
	}
	free(node);
	freecnt++;
    }

    return;
}


//  function used internally to 'base' a prefix
static ipv4addr_t
PT_basePrefix ( ipv4addr_t addr, uint8_t mb )
{
    ipv4addr_t  mask;
    
    mask  = 0xffffffff;
    mask  = mask >> (32 - mb) << (32 - mb);

    addr &= htonl(mask);

    return addr;
}


//  callback function used to find the longest match
static void
PT_searchLongHandler ( ptNode_t * node )
{
    int  i;
    
    for ( i = 0; i < (MAX_MASKLEN + 1); i++ ) {
	if ( (node->rocks[i]) && 
	     (PT_basePrefix(searchCidr.addr, i) == node->key) ) {
	    if ( i > resultCidr.addrlen ) {
		resultCidr.addr    = node->key;
		resultCidr.addrlen = i;
	    }
	}
    }
    return;
}


//  callback used by pt_nodes() to count allocated nodes
static void
PT_countNodesHandler ( ptNode_t * node )
{
    nodecnt++;
    return;
}


//  callback used by pt_size() to count allocated children
static void
PT_countRocksHandler ( ipv4addr_t addr, uint8_t mb, void * rock )
{
    if ( rock )
	ptsize++;
    return;
}



/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- 
 *  public functions for working with the trie
 *
 */

//  create and initialize a new trie
ptNode_t*
pt_init()
{
    ptNode_t * head;
    cidr_t     cidr;

    cidr.addr    = 0;
    cidr.addrlen = 0;

    head = PT_new(cidr, -1, NULL, NULL, NULL);

    if ( head != NULL ) {
	head->llink = head;
	head->rlink = head;
    }

    memset(&searchCidr, 0, sizeof(cidr_t));
    memset(&resultCidr, 0, sizeof(cidr_t));

    return head;
}


//  inserts a node into the trie.
//  Returns 1 on success, 0 on error.
int
pt_insert ( ptNode_t * head, cidr_t cidr, void* rock )
{
    ptNode_t  * node;
    int         bit;
    int         result = 0;

    node = PT_searchR(head->llink, cidr.addr, -1);

    if ( cidr.addr != node->key ) {
	result = 1;
	for ( bit = 0; GETBIT(cidr.addr,bit) == GETBIT(node->key, bit); bit++ );
	head->llink = PT_insertR(head->llink, cidr, bit, head, rock);
    } else if ( ! node->rocks[cidr.addrlen] ) {
	result = 1;
	node->rocks[cidr.addrlen] = rock;
    }
	
    return result;
}


//  returns 1 if the provided key has an exact match in the trie.
int
pt_match ( ptNode_t * head, cidr_t cidr )
{
    ptNode_t  * node;

    node = PT_searchR(head->llink, cidr.addr, -1);

    if ( node->key == cidr.addr && (node->rocks[cidr.addrlen]) )
	return 1;

    return 0;
}


//  function to provide an exact match to the provided key
//  and return the associated void*, or NULL if there was no match
void*
pt_matchRock ( ptNode_t * head, cidr_t cidr )
{
    ptNode_t  * node;
    void      * rock = NULL;

    node = PT_searchR(head->llink, cidr.addr, -1);

    if ( node->key == cidr.addr && (node->rocks[cidr.addrlen]) )
	rock = node->rocks[cidr.addrlen];

    return rock;
}


void*
pt_matchLongest ( ptNode_t * head, cidr_t cidr )
{
    void * rock = NULL;

    searchCidr.addr    = PT_basePrefix(cidr.addr, cidr.addrlen);
    searchCidr.addrlen = cidr.addrlen;

    memset(&resultCidr, 0, sizeof(cidr_t));

    PT_visitR_node(head->llink, -1, &PT_searchLongHandler);

    if ( resultCidr.addr > 0 )
	rock = pt_matchRock(head, resultCidr);

    return rock;
}


void*
pt_remove ( ptNode_t * head, cidr_t cidr )
{
    void  * rock = NULL;

    rock = PT_removeR(head->llink, cidr, -1);

    return rock;
}


//  Visits nodes of the trie in order 
void
pt_visit ( ptNode_t * head, nodeHandler_t handler )
{
    if ( head->llink != NULL )
	PT_visitR(head->llink, -1, handler);
}



int
pt_nodes ( ptNode_t * head )
{
    nodecnt = 0;
    if ( head->llink != NULL )
	PT_visitR_node(head->llink, -1, &PT_countNodesHandler);
    else
	return -1;
    return nodecnt;
}


int
pt_size ( ptNode_t * head )
{
    ptsize = 0;
    pt_visit(head, &PT_countRocksHandler);
    return ptsize;
}


//  Frees the nodes of a patricia trie
int
pt_free ( ptNode_t * head, nodeHandler_t handler )
{
    freecnt = 0;

    PT_freeNodesR(head, head->llink, -1, handler);
    head->llink = head->rlink = head;

    return freecnt;
}



//  PATRICIA_C

