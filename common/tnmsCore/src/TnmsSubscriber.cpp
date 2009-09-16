#define _TNMSCORE_TNMSSUBSCRIBER_CPP_

#include "TnmsSubscriber.h"


namespace tnmsCore {


TnmsSubscriber::TnmsSubscriber()
    : TreeSubscriber()
{
}

TnmsSubscriber::~TnmsSubscriber()
{
}


void
TnmsSubscriber::queueAdd ( TnmsTree::Node * node )
{
    if ( removes.erase(node->getValue().metric.getElementName()) )
        return;
    adds.insert(node);
}

void
TnmsSubscriber::queueRemove ( TnmsTree::Node * node )
{
    adds.erase(node);
    updates.erase(node);
    removes.insert(node->getValue().metric.getElementName());
}


void
TnmsSubscriber::queueUpdate ( TnmsTree::Node * node )
{
    updates.insert(node);
}

bool
TnmsSubscriber::haveUpdates() const
{
    if ( (adds.size() + removes.size() + updates.size()) > 0 )
        return true;
    return false;
}


}  // namespace


// _TNMSCORE_TNMSSUBSCRIBER_CPP_
