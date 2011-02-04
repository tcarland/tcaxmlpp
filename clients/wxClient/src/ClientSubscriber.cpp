#define _CLIENTSUBSCRIBER_CPP_

#include "ClientSubscriber.h"

namespace tnmsClient {


bool
ClientSubscriber::queueAdd ( TnmsTree::Node * node )
{
    if ( this->lock() <= 0 )
        return false;

    TnmsSubscriber::queueAdd(node);

    this->unlock();

    return true;
}


bool
ClientSubscriber::queueUpdate ( TnmsTree::Node * node )
{
    if ( ! this->lock() )
        return false;

    TnmsSubscriber::queueUpdate(node);

    this->unlock();

    return true;
}


bool
ClientSubscriber::queueRemove ( TnmsTree::Node * node )
{
    if ( ! this->lock() )
        return false;

    TnmsSubscriber::queueRemove(node);

    this->unlock();

    return true;
}


//-------------------------
//  The subscriber's add/update/remove queues are public
//  so the reader (only) must lock the mutex prior to 
//  iterating on any of the queues.
int
ClientSubscriber::lock()
{
    return _mutex.lock();
}


int
ClientSubscriber::trylock()
{
    return _mutex.tryLock();
}


int
ClientSubscriber::unlock()
{
    return _mutex.unlock();
}

} // namespace

// _CLIENTSUBSCRIBER_CPP_

