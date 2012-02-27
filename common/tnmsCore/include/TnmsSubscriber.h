/*
 * TreeSubscriber.h
 *
 *  Copyright(c) 2008,2009  Timothy Charlton Arland
 *  Author: tca@charltontechnology.net
 */
#ifndef _TNMSCORE_TNMSSUBSCRIBER_H_
#define _TNMSCORE_TNMSSUBSCRIBER_H_

#include <string>
#include <set>

#include "TreeSubscriber.hpp"

namespace tnmsCore {


typedef std::set<TnmsTree::Node*>  TreeUpdateSet;
typedef std::set<std::string>      TreeRemoveSet;



class TnmsSubscriber : public TreeSubscriber {

  public:

    TnmsSubscriber() {}

    virtual ~TnmsSubscriber() {}

    virtual bool  queueAdd    ( TnmsTree::Node * node );
    virtual bool  queueUpdate ( TnmsTree::Node * node );
    virtual bool  queueRemove ( TnmsTree::Node * node );

    virtual bool  haveUpdates() const;

  public:

    TreeUpdateSet        adds;
    TreeUpdateSet        updates;
    TreeRemoveSet        removes;

};



}  // namespace


#endif  //_TNMSCORE_TNMSSUBSCRIBER_H_
