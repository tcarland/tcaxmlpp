#define _CLIENTSUBSCRIBER_CPP_

#include "ClientSubscriber.h"


ClientSubscriber::ClientSubscriber() {}

ClientSubscriber::~ClientSubscriber() {}


void
ClientSubscriber::queueAdd ( TnmsTree::Node * node )
{
    if ( this->trylock() <= 0 )
        return;

    TnmsSubscriber::queueAdd(node);

    this->unlock();

    return;
}

void
ClientSubscriber::queueUpdate ( TnmsTree::Node * node )
{
    if ( ! this->trylock() )
        return;

    TnmsSubscriber::queueUpdate(node);

    this->unlock();

    return;
}

void
ClientSubscriber::queueRemove ( TnmsTree::Node * node )
{
    if ( ! this->trylock() )
        return;

    TnmsSubscriber::queueRemove(node);

    this->unlock();

    return;
}

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


// _CLIENTSUBSCRIBER_CPP_

