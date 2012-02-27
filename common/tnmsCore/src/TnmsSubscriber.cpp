/*
 *  TreeSubscriber.cpp
 *
 *  Copyright(c) 2008,2009  Timothy Charlton Arland
 *  Author: tca@charltontechnology.net
 */
#define _TNMSCORE_TNMSSUBSCRIBER_CPP_

#include "TnmsSubscriber.h"


namespace tnmsCore {



bool
TnmsSubscriber::queueAdd ( TnmsTree::Node * node )
{
    if ( removes.erase(node->getValue().metric.getElementName()) )
        return true;
    adds.insert(node);
    return true;
}

bool
TnmsSubscriber::queueRemove ( TnmsTree::Node * node )
{
    adds.erase(node);
    updates.erase(node);
    removes.insert(node->getValue().metric.getElementName());
    return true;
}


bool
TnmsSubscriber::queueUpdate ( TnmsTree::Node * node )
{
    updates.insert(node);
    return true;
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
