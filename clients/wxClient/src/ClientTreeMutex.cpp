#define _TNMSCLIENT_CLIENTTREEMUTEX_CPP_

#include "ClientTreeMutex.h"
#include "ClientSubscriber.h"


namespace tnmsclient {


ClientTreeMutex::ClientTreeMutex()
    : _subscriber(new ClientSubscriber()),
      _tree(new TnmsTree())
{}

ClientTreeMutex::ClientTreeMutex ( ClientSubscriber * subscriber_, TnmsTree * tree_ )
    : _subscriber(subscriber_),
      _tree(tree_)
{}

ClientTreeMutex::~ClientTreeMutex()
{
    if ( tree )
        delete _tree;
    if ( notifier )
        delete _subscriber;
}

TnmsTree*
ClientTreeMutex::acquireTree()
{
    TnmsTree * t = NULL;

    if ( this->lock() )
        t = this->_tree;

    return t;
}

void
ClientTreeMutex::releaseTree()
{
    this->unlock();
    this->notifyAll();
}

ClientSubscriber*
ClientTreeMutex::subscriber()
{
    return _subscriber;
}


} // namespace

// _TNMSCLIENT_CLIENTTREEMUTEX_CPP_

