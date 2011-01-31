#ifndef _TNMSCLIENT_CLIENTTREEMUTEX_H_
#define _TNMSCLIENT_CLIENTTREEMUTEX_H_

#include "TnmsTree.h"
using namespace tnmsCore;
#include "ThreadLock.h"
using namespace tcanetpp;


namespace tnmsclient {


class ClientSubscriber;


class ClientTreeMutex : public ThreadLock {

  public:

    ClientTreeMutex();
    ClientTreeMutex ( ClientSubscriber    * subscriber,
                      tnmsCore::TnmsTree  * tree )

    virtual ~ClientTreeMutex();


    TnmsTree*          acquireTree();
    void               releaseTree();

    ClientSubscriber*  subscriber();


  protected:

    ClientSubsriber  * _subscriber;
    TnmsTree         * _tree;
    
};

} // namespace 

#endif  //  _TNMSCLIENT_CLIENTTREEMUTEX_H_
