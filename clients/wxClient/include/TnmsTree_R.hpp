#ifndef _TNMSTREE_R_HPP_
#define _TNMSTREE_R_HPP_


#include "TnmsTree.h"
using namespace tnmsCore;

#include "ThreadLock.h"
using namespace tcanetpp;


struct TnmsTree_R
{
    TnmsTree   * tree;
    ThreadLock * mutex;

    TnmsTree_R ( TnmsTree   * tree_ = NULL,
                 ThreadLock * lock_ = NULL )
        : tree(tree_), 
          mutex(lock_)
    {}
};


#endif  // _TNMSTREE_R_HPP_

