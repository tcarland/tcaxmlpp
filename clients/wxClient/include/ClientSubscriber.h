#ifndef _CLIENTSUBSCRIBER_H_
#define _CLIENTSUBSCRIBER_H_

#include "TnmsSubscriber.h"
using namespace tnmsCore;

#include "ThreadLock.h"
using namespace tcanetpp;

namespace tnmsClient {

class ClientSubscriber : public TnmsSubscriber {

  public:

    ClientSubscriber() {}
    virtual ~ClientSubscriber() {}

    virtual bool  queueAdd    ( TnmsTree::Node * node );
    virtual bool  queueUpdate ( TnmsTree::Node * node );
    virtual bool  queueRemove ( TnmsTree::Node * node );

    int           lock();
    int           trylock();
    int           unlock();
  
  private:

    ThreadLock   _mutex;

};

} // namespace

#endif  //_CLIENTSUBSCRIBER_H_

