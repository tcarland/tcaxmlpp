/**  
  *  TreeSubscriber Interface
  *
  *     Acts as a metric queue of updates from the TnmsTree. 
  *  On interval or processing of the tree, the TnmsTree::updateSubscribers
  *  method will process the updates to all appropriate subscribers calling 
  *  these three pure virtual methods of the corresponding subscriber instance.
 **/
#ifndef _TNMSCORE_TREESUBSCRIBER_HPP_
#define _TNMSCORE_TREESUBSCRIBER_HPP_


#include "TnmsTree.h"


namespace tnmsCore {

class TnmsTree;


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

