#ifndef _TNMSTREE_R_HPP_
#define _TNMSTREE_R_HPP_


#include "TnmsTree.h"
using namespace tnmsCore;

#include "ThreadLock.h"
using namespace tcanetpp;


class TnmsClientIOThread;

struct TnmsTree_R
{
    TnmsClientIOThread  * iomgr;
    TnmsTree            * tree;
    ThreadLock          * mutex;

    bool         updated;

    TnmsTree_R ( TnmsClientIOThread * iomgr_ = NULL,
                 TnmsTree           * tree_  = NULL,
                 ThreadLock         * lock_  = NULL )
        : iomgr(iomgr_),
          tree(tree_), 
          mutex(lock_)
    {}
};


#endif  // _TNMSTREE_R_HPP_

