/**  A Patricia Trie implementation based on Sedgewick's 
  *  "Algorithm's in C". The original implementation has been
  *  extended to allow for a void* (rock) to be stored per node.
  *  Thanks to Brian Grady for providing some of the initial  
  *  Sedgewick implementation (pt_insert and remove).
  *
  *     Note that some functions (notably longestMatch) make
  *  use of static variables, which can cause concurrancy
  *  issues in a threaded environment, especially with multiple
  *  patricia instances. If thread safety is needed, care should
  *  be exercised taken to properly wrap function calls around a
  *  mutex.
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
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


#define GETBIT(v, bit)  ((v) & (0x80000000 >> (bit)))
#define PT_DELETE_FLAG  0x1
#define MAX_MASKLEN     32


typedef struct ptNode {
    uint32_t        key;
    int             bit;
    uint32_t        flags;
    struct ptNode*  llink;
    struct ptNode*  rlink;
    void*           rocks[MAX_MASKLEN + 1];
} ptNode_t;


typedef void (*nodeHandler_t) (uint32_t, uint8_t, void*);
typedef void (*pvtNodeHandler_t)(ptNode_t*);


ptNode_t*  pt_init();

int        pt_insert       ( ptNode_t * head, cidr_t cidr, void * rock );
void*      pt_remove       ( ptNode_t * head, cidr_t key );

int        pt_match        ( ptNode_t * head, cidr_t key );
void*      pt_matchForRock ( ptNode_t * head, cidr_t key );
void*      pt_matchLongest ( ptNode_t * head, cidr_t key );

void       pt_visit        ( ptNode_t * head, nodeHandler_t handler );
void       pt_visit_node   ( ptNode_t * head, pvtNodeHandler_t handler );
 
int        pt_nodes        ( ptNode_t * head );
int        pt_size         ( ptNode_t * head );

int        pt_free         ( ptNode_t * head, nodeHandler_t handler );


# ifdef __cplusplus
}  // extern "C"
# endif


#endif  //  _TCANETPP_PATRICIA_H_

