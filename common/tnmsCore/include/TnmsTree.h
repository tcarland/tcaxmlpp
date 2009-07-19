#ifndef _TNMSD_TNMSTREE_H_
#define _TNMSD_TNMSTREE_H_

#include <set>
#include <list>
#include <map>

#include "TnmsMetric.h"
#include "HeirarchicalStringTree.hpp"


namespace tnmsCore {

class TnmsSubscriber;


class TnmsTree {

  public:

    typedef std::set<TnmsSubscriber*>   NotifySet;

    struct TnmsTreeNode
    {
        TnmsMetric  metric;
        NotifySet   nodeSubscribers;
        NotifySet   levelSubscribers;
        bool        erase;

        TnmsTreeNode()
            : erase(false)
        {}
    };

    typedef HeirarchicalStringTree<TnmsTreeNode>        Tree;
    typedef Tree::Node                                  Node;
    typedef Tree::NodeMap                               NodeChildMap;

    typedef std::set<Node*>                             NodeSet;
    typedef std::set<std::string>                       StringSet;
    typedef std::map<std::string, Node*>                NameNodeMap;


  public:

    TnmsTree();
    virtual ~TnmsTree();

    bool       add              ( const std::string  & name );
    bool       remove           ( const std::string  & name );
    bool       update           ( const TnmsMetric   & metric );

    bool       request          ( const std::string  & name,
                                 TnmsSubscriber      * sub );
    bool       request          ( const std::string  & name,
                                  TnmsMetric         & metric );

    bool       exists           ( const std::string  & name );

    bool       subscribe        ( const std::string  & name, 
                                  TnmsSubscriber     * sub );
    bool       unsubscribe      ( const std::string  & name, 
                                  TnmsSubscriber     * sub );

    bool       subStructure     ( TnmsSubscriber     * sub );
    bool       unsubStructure   ( TnmsSubscriber     * sub );

    void       updateSubscribers();
    void       removeSubscriber ( TnmsSubscriber     * sub );

    void       sweep();
    void       sweep            ( TnmsTree::Node     * node );
    void       sweep            ( const std::string  & name );

    size_t     size() const;
    void       clear();

    void       debugDump() const;
    void       debugDump        ( const std::string  & name ) const;

    void       getRootNames     ( StringSet & rootnames ) const;

  protected:

    bool       markForRemove    ( const std::string  & name );
    void       clearNodeErase   ( TnmsTree::Node     * node );

    void       resetMetric      ( TnmsMetric         & metric );


  private:

    Tree  *             _tree;
    NotifySet           _structureSubs;
    NotifySet           _allSubs;
    NotifySet           _rootSubs;

    NameNodeMap         _updates;
};

}  // namespace

#endif  //  _TNMSD_TNMSTREE_H_
