/**
  * @file patricia.c
  *
  * Copyright (c) 2002,2008 Timothy Charlton Arland 
  * @author  tcarland@gmail.com
  *
  * @section LICENSE
  *
  * This file is part of tcanetpp.
  *
  * tcanetpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcanetpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcanetpp.  
  * If not, see <http://www.gnu.org/licenses/>.
**/
#define _TCANETPP_PATRICIA_C_


#include <stdlib.h>
#include <string.h>

#include "patricia.h"


static const
char version[] = "$Id: patricia.c,v 1.91 2011/03/06 12:27:19 tca$";


static cidr_t searchCidr;
static cidr_t resultCidr;
static int    freecnt    = 0;
static int    nodecnt    = 0;
static int    ptsize     = 0;


//  Create a new node
static ptNode_t*
PT_new ( cidr_t     cidr, 
         int        bit, 
         ptNode_t * llink, 
         ptNode_t * rlink, 
         void     * rock )
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
        np->rocks[cidr.mb] = rock;

    return np;
}


//  Visits all nodes and children of the trie.
static void
PT_visitR ( ptNode_t * node, int bit, nodeHandler_t handler )
{
    int i;

    if ( node->bit <= bit ) 
    {
        for ( i = 0; i < PT_MASKLEN; i++ ) 
        {
            if ( (node->rocks[i]) && handler ) {
                handler(node->key, i, node->rocks[i]);
            }
        }
    } 
    else 
    {
        PT_visitR(node->llink, node->bit, handler);
        PT_visitR(node->rlink, node->bit, handler);
    }
    return;
}


//  Visits all nodes of the trie.
static void
PT_visitR_node ( ptNode_t * node, int bit, pvtNodeHandler_t handler )
{
    if ( node->bit <= bit ) 
    {
        if ( handler ) 
            handler(node);
    } 
    else 
    {
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

    if ( PT_GETBIT(key, node->bit) == 0 )
        return PT_searchR(node->llink, key, node->bit);
    
    return PT_searchR(node->rlink, key, node->bit);
}


//  recursive insert into the trie 
static ptNode_t*
PT_insertR ( ptNode_t * head, 
             cidr_t     cidr, 
             int        bit, 
             ptNode_t * p, 
             void     * rock )
{
    ptNode_t * node = NULL;

    if ( (head->bit >= bit) || (head->bit <= p->bit) ) 
    {
        node = PT_new(cidr, bit, 0, 0, rock);

        if ( PT_GETBIT(cidr.addr, bit) ) {
            node->llink = head;
            node->rlink = node;
        } else {
            node->llink = node;
            node->rlink = head;
        }

        return node;
    }

    if ( PT_GETBIT(cidr.addr, head->bit) == 0 ) {
        head->llink = PT_insertR(head->llink, cidr, bit, head, rock);
    } else {
        head->rlink = PT_insertR(head->rlink, cidr, bit, head, rock);
    }

    return head;
}


//  recursive removal from the trie
static void*
PT_removeR ( ptNode_t * node, cidr_t cidr, int bit )
{
    void * rock = NULL;

    if ( node->bit <= bit ) 
    {
        if ( node->key == cidr.addr && node->rocks[cidr.mb] ) {
            rock = node->rocks[cidr.mb];
            node->rocks[cidr.mb] = NULL;
        }
        return rock;
    }
    
    if ( PT_GETBIT(cidr.addr, node->bit) == 0 )
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

    if ( node != head ) 
    {
        for ( i = 0; i < PT_MASKLEN; i++ ) 
        {
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
PT_basePrefix ( ipv4addr_t addr, uint16_t mb )
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
    
    for ( i = 0; i < (PT_MASKLEN + 1); i++ ) 
    {
        if ( node->rocks[i] && PT_basePrefix(searchCidr.addr, i) == node->key )  
        {
            if ( i > resultCidr.mb ) {
                resultCidr.addr = node->key;
                resultCidr.mb   = i;
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
PT_countRocksHandler ( ipv4addr_t addr, uint16_t mb, void * rock )
{
    if ( rock )
        ptsize++;
    return;
}

//--------------------------------------------------------------------


/**  Initialize a new trie */
ptNode_t*
pt_init()
{
    ptNode_t * head;
    cidr_t     cidr;

    cidr.addr = 0;
    cidr.mb   = 0;

    head = PT_new(cidr, -1, NULL, NULL, NULL);

    if ( head != NULL ) {
        head->llink = head;
        head->rlink = head;
    }

    memset(&searchCidr, 0, sizeof(cidr_t));
    memset(&resultCidr, 0, sizeof(cidr_t));

    return head;
}


/**  Inserts a node into the trie. Returns 1 on success, 0 on error. */
int
pt_insert ( ptNode_t * head, cidr_t cidr, void * rock )
{
    ptNode_t  * node;
    int         bit;
    int         result = 0;

    node = PT_searchR(head->llink, cidr.addr, -1);

    if ( cidr.addr != node->key ) 
    {
        result = 1;
        for ( bit = 0; PT_GETBIT(cidr.addr,bit) == PT_GETBIT(node->key, bit); bit++ );
        head->llink = PT_insertR(head->llink, cidr, bit, head, rock);
    } 
    else if ( ! node->rocks[cidr.mb] )
    {
        result = 1;
        node->rocks[cidr.mb] = rock;
    }
        
    return result;
}


/**  Searches the trie for the ip prefix. Returns 1 if the
  *  provided key has an exact match in the trie or 0 if it
  *  does not exist. (pt_match is usually a better usage choice)
  *  @param head  is a pointer to patricia node to check.
  *  @param cidr  is the IP Prefix to check.
 **/
int
pt_exists ( ptNode_t * head, cidr_t cidr )
{
    ptNode_t  * node;

    node = PT_searchR(head->llink, cidr.addr, -1);

    if ( node->key == cidr.addr && (node->rocks[cidr.mb]) )
        return 1;

    return 0;
}


/**  Function to provide an exact match to the provided key.
  *  @param head  is the node from which to match. 
  *  @param cidr  is the IP Prefix to match.
  *  Returns the associated void*, or NULL if there is no match.
 **/
void*
pt_match ( ptNode_t * head, cidr_t cidr )
{
    ptNode_t  * node;
    void      * rock = NULL;

    node = PT_searchR(head->llink, cidr.addr, -1);

    if ( node->key == cidr.addr && (node->rocks[cidr.mb]) )
        rock = node->rocks[cidr.mb];

    return rock;
}

/**  Searches the trie, for the longest (bit mask) ip addr
  *  that encompasses the provided network address.
 **/
void*
pt_matchLongest ( ptNode_t * head, cidr_t cidr )
{
    void * rock = NULL;

    searchCidr.addr = PT_basePrefix(cidr.addr, cidr.mb);
    searchCidr.mb   = cidr.mb;

    memset(&resultCidr, 0, sizeof(cidr_t));

    PT_visitR_node(head->llink, -1, &PT_searchLongHandler);

    if ( resultCidr.addr > 0 )
        rock = pt_match(head, resultCidr);

    return rock;
}

/**  Removes the provided IP Prefix from the trie
  *  returning the associated user data as a void*.
  *  @param head  is the root node from which to remove.
  *  @param cidr  is the IP Prefix to remove.
 **/
void*
pt_remove ( ptNode_t * head, cidr_t cidr )
{
    void  * rock = NULL;

    rock = PT_removeR(head->llink, cidr, -1);

    return rock;
}


/**  Visits nodes of the trie in order. */
void
pt_visit ( ptNode_t * head, nodeHandler_t handler )
{
    if ( head->llink != NULL )
        PT_visitR(head->llink, -1, handler);
}

/**  Visits nodes of the trie in order */
void
pt_visit_node ( ptNode_t * head, pvtNodeHandler_t handler )
{
    if ( head->llink != NULL )
        PT_visitR_node(head->llink, -1, handler);
}

/**  Returns the number of nodes in the trie.
  *  Each node can have multiple entries.
 **/
int
pt_nodes ( ptNode_t * head )
{
    nodecnt = 0;
    
    if ( head->llink != NULL ) {
        PT_visitR_node(head->llink, -1, &PT_countNodesHandler);
    } else {
        return -1;
    }
    
    return nodecnt;
}


/**  Returns the total number of entries in the trie.
  *  Note this differs from the actual number of nodes in the trie.
 **/
int
pt_size ( ptNode_t * head )
{
    ptsize  = 0;

    pt_visit(head, &PT_countRocksHandler);

    return ptsize;
}


/**  Free nodes of a patricia trie */
int
pt_free ( ptNode_t * head, nodeHandler_t handler )
{
    freecnt = 0;

    PT_freeNodesR(head, head->llink, -1, handler);
    head->llink = head->rlink = head;

    return freecnt;
}


//  _TCANETPP_PATRICIA_C_

