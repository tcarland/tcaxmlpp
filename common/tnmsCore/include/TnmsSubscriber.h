#ifndef _TNMSCORE_TNMSSUBSCRIBER_H_
#define _TNMSCORE_TNMSSUBSCRIBER_H_

#include <string>
#include <set>

#include "TnmsTree.h"

namespace tnmsCore {


typedef std::set<TnmsTree::Node*>  TreeUpdateSet;
typedef std::set<std::string>      TreeRemoveSet;


class TnmsSubscriber {

  public:

    TnmsSubscriber();

    virtual ~TnmsSubscriber();

    virtual void  queueAdd    ( TnmsTree::Node * node );
    virtual void  queueUpdate ( TnmsTree::Node * node );
    virtual void  queueRemove ( TnmsTree::Node * node );

    virtual bool  haveUpdates() const;

  public:

    TreeUpdateSet        adds;
    TreeUpdateSet        updates;
    TreeRemoveSet        removes;
};



}  // namespace


#endif  //_TNMSCORE_TNMSSUBSCRIBER_H_
