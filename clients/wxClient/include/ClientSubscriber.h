#ifndef _CLIENTSUBSCRIBER_H_
#define _CLIENTSUBSCRIBER_H_


#include "TnmsSubscriber.h"
using namespace tnmsCore;

#include "ThreadLock.h"
using namespace tcanetpp;


class ClientSubscriber : public TnmsSubscriber {

  public:

    ClientSubscriber();
    virtual ~ClientSubscriber();

    virtual void  queueAdd    ( TnmsTree::Node * node );
    virtual void  queueUpdate ( TnmsTree::Node * node );
    virtual void  queueRemove ( TnmsTree::Node * node );

    int           lock();
    int           trylock();
    int           unlock();
  
  private:

    ThreadLock   _mutex;

};


#endif  //_CLIENTSUBSCRIBER_H_

