/**
  * @file patricia.h
  *
  *    A Radix Trie implementation.
  *
  *    Note that some functions (notably longestMatch) make
  * use of static variables, which can cause concurrancy
  * issues in a threaded environment.
  *    If thread safety is needed, care should be exercised
  * to properly synchronize function calls.
  *
  * Copyright (c) 2002,2008,2013 Timothy Charlton Arland
  * @author tcarland@gmail.com
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
#ifndef _TCANETPP_PATRICIA_H_
#define _TCANETPP_PATRICIA_H_

#include "tcanetpp_types.h"

# ifdef __cplusplus
extern "C" {
# endif
#include <stdlib.h>


#define PT_GETBIT(v, bit)  ((v) & (0x8000000000000000 >> (bit)))
#define PT_DELETE_FLAG     0x1
#define PT_MASKLEN         64
#define PT_MAXBITS         64


typedef struct ptNode {
    uint64_t        key;
    uint64_t        host;   // host == key if node is ipv4
    int             bit;
    uint32_t        flags;
    struct ptNode*  llink;
    struct ptNode*  rlink;
    uint32_t        slots;
    void*           rocks[PT_MASKLEN];
} ptNode_t;


/** Typical node handler used when walking the trie via pt_visit() */
typedef void (*nodeHandler_t) (uint64_t, uint64_t, uint16_t, void*);

/** Specialized node handler used for obtaining a node pointer */
typedef void (*pvtNodeHandler_t)(ptNode_t*);



ptNode_t*   pt_init();

int         pt_insert       ( ptNode_t * head, prefix_t * pfx, void * rock );
void*       pt_remove       ( ptNode_t * head, prefix_t * key );
int         pt_exists       ( ptNode_t * head, prefix_t * key );

void*       pt_match        ( ptNode_t * head, prefix_t * key );
void*       pt_matchLongest ( ptNode_t * head, prefix_t * key );

void        pt_visit        ( ptNode_t * head, nodeHandler_t handler );
void        pt_visit_node   ( ptNode_t * head, pvtNodeHandler_t handler );
 
int         pt_nodes        ( ptNode_t * head );
int         pt_size         ( ptNode_t * head );

int         pt_free         ( ptNode_t * head, nodeHandler_t handler );

int         pt_is_ipv4      ( ptNode_t * node );
ipv4addr_t  pt_to_ipv4      ( ptNode_t * node );

const char* pt_version();


# ifdef __cplusplus
}  // extern "C"
# endif


#endif  //  _TCANETPP_PATRICIA_H_

