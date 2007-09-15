/**  A Patricia Trie implementation based on Sedgewick's 
  *  "Algorithm's in C". The original implementation has been
  *  extended to allow for a void* (rock) to be stored per node.
  *  Thanks to Brian Grady for providing some of the initial  
  *  Sedgewick implementation (pt_insert and remove).
  *
  *     The current method of storing these void*'s is not 
  *  the most space considerate method, requiring 132 bytes
  *  of storage per node. Additionally, this requires valid 
  *  (rock) pointers to be provided on insert for the patricia 
  *  to function properly.
  *
  *     Note that some functions (notably longestMatch) make
  *  use of static variables, which can cause concurrancy
  *  issues in a threaded environment, especially with multiple
  *  patricia instances. If thread safety is needed, care should
  *  be exercised taken to properly wrap function calls around a
  *  mutex.
  *
  *  @Author   tcarland@gmail.com
  *  @Version  1.1
  *
  *  $Id: patricia.h,v 1.12 2003/10/03 21:27:19 tca Exp $
 **/
#ifndef _TCANETPP_PATRICIA_H_
#define _TCANETPP_PATRICIA_H_

#include "tcanetpp_types.h"


# ifdef __cplusplus
extern "C" {
# endif


#define GETBIT(v, bit) ((v) & (0x80000000 >> (bit)))

#define PT_DELETE_FLAG 0x1
#define MAX_MASKLEN    32


/* Patricia node structure. the rocks array uses a bit more space 
 * than desired, but it was quicker and easier to pre-allocate
 * the space for the void*'s for each possible mask.
 */
typedef struct ptNode {
    uint32_t        key;
    int             bit;
    uint32_t        flags;
    struct ptNode*  llink;
    struct ptNode*  rlink;
    void*           rocks[MAX_MASKLEN + 1];
} ptNode_t;


/*  Defines the function callback prototype required for
 *  node visitation needed by the sort and free functions.
 */
typedef void (*nodeHandler_t) (uint32_t, uint8_t, void*);

/*  Special handler that takes a node pointer for performing 
 *  some function during visitation. For instance, a version of
 *  this can be used for debug purposes (See pt_visit_debug).
 */
typedef void (*pvtNodeHandler_t)(ptNode_t*);



/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 *  pt function prototypes
 */


/**  Initializes and returns a new trie */
ptNode_t*  pt_init();


/**  Inserts a new key into the trie */
int   pt_insert ( ptNode_t* head, cidr_t cidr, void* rock );


/**  Performs an exact match and returns 1 on success
  *  or 0 if not a match.
 **/
int   pt_match ( ptNode_t* head, cidr_t key );


/**  Same as pt_match() except the function returns the associated void*
  *  or NULL if not a match.
 **/
void* pt_matchRock ( ptNode_t* head, cidr_t key );


/**  Returns the void* to the longest matching prefix found in the 
  *  patricia or NULL if none is found. 
 **/
void* pt_matchLongest ( ptNode_t* head, cidr_t key );


/**  Marks a node as deleted and returns the void* rock */
void* pt_remove ( ptNode_t* head, cidr_t key );


/**  Visits each node in the trie using the user
  *  supplied callback function.
 **/
void  pt_visit ( ptNode_t* head, nodeHandler_t handler );


/**  Function that visits each node of the trie and gives the
  *  provided handler full access to the underlying node ptr.
 **/
void  pt_visit_debug ( ptNode_t* head, pvtNodeHandler_t handler );


/**  Returns the number of allocated nodes */
int   pt_nodes ( ptNode_t* head );


/**  Walks the entire trie and counts the number of allocated 
  *  allocated subnets by counting the void* rocks.
 **/
int   pt_size ( ptNode_t* head );


/**  Frees all nodes of a trie and returns the number of nodes freed.
  *
  *  Note that this function does not free any user allocated 
  *  rock's (void* objects) associated with each node, and must instead
  *  provide a node handler to properly free each associated object.
  *  
  *  The following is a simple example of such a handler:
  *
  *   void nodeHandler ( void* rock )
  *   {
  *      MyObject* foo = (MyObject*) rock;
  *      foo->postProcess();
  *      free(foo);  // or delete as the case may be
  *   }
 **/
int  pt_free ( ptNode_t* head, nodeHandler_t handler );


# ifdef __cplusplus
}  // namespace
# endif


#endif  //  _TCANETPP_PATRICIA_H_
