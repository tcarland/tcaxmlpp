#ifndef _TNMSTREE_R_HPP_
#define _TNMSTREE_R_HPP_


#include "TnmsTree.h"
using namespace tnmsCore;

#include "ThreadLock.h"
using namespace tcanetpp;


class TnmsClientIOThread;
class ClientSubscriber;



struct TnmsTree_R
{
    TnmsClientIOThread  * iomgr;  
    ClientSubscriber    * notifier;
    TnmsTree            * tree;
    ThreadLock          * mutex;

    TnmsTree_R ( TnmsClientIOThread * iomgr_   = NULL,
                 ClientSubscriber   * notify_  = NULL,
                 TnmsTree           * tree_    = NULL,
                 ThreadLock         * lock_    = NULL )
        : iomgr(iomgr_),
          notifier(notify_),
          tree(tree_), 
          mutex(lock_)
    {}


    TnmsTree*  lockTree()
    {
        if ( mutex == NULL || tree == NULL )
            return NULL;
        mutex->lock();
        return tree;
    }

    void unlockTree()
    {
        if ( mutex == NULL )
            return;
        mutex->unlock();
    }
};


#endif  // _TNMSTREE_R_HPP_

