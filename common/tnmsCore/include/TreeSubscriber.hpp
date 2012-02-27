/*
 *  TreeSubscriber.hpp
 *
 *  Copyright(c) 2008,2009  Timothy Charlton Arland
 *  Author: tca@charltontechnology.net
 */
#ifndef _TNMSCORE_TREESUBSCRIBER_HPP_
#define _TNMSCORE_TREESUBSCRIBER_HPP_


#include "TnmsTree.h"


namespace tnmsCore {

class TnmsTree;

/**  TreeSubscriber
  *
  *     Acts as a metric queue for updates from the TnmsTree.
  *  On interval or processing of the tree, the TnmsTree::updateSubscribers
  *  method will process the updates to all appropriate subscribers calling
  *  these three pure virtual methods of the corresponding subscriber instance.
 **/
class TreeSubscriber {

  public:

    TreeSubscriber() {}
    virtual ~TreeSubscriber() {}

    virtual bool   queueAdd    ( TnmsTree::Node * node ) = 0;
    virtual bool   queueUpdate ( TnmsTree::Node * node ) = 0;
    virtual bool   queueRemove ( TnmsTree::Node * node ) = 0;

};


}  // namespace

#endif  // _TNMSCORE_TREESUBSCRIBER_HPP_

