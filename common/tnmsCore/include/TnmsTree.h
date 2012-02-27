/*
 * TnmsTree.h
 *
 *  Copyright(c) 2008, 2009  Timothy Charlton Arland
 *  Author: tca@charltontechnology.net
 */
#ifndef _TNMSCORE_TNMSTREE_H_
#define _TNMSCORE_TNMSTREE_H_

#include <set>
#include <list>
#include <map>

#include "TnmsMetric.h"

#include "HeirarchicalStringTree.hpp"
using namespace tcanetpp;


namespace tnmsCore {


class TreeSubscriber;


class TnmsTree {

  public:

    typedef std::set<TreeSubscriber*>   NotifySet;
    

    struct TnmsTreeNode
    {
        TnmsMetric    metric;
        NotifySet     nodeSubscribers;
        NotifySet     levelSubscribers;
        bool          erase;

        TnmsTreeNode()
            : erase(false)
        {}
    };

    typedef HeirarchicalStringTree<TnmsTreeNode>    Tree;
    typedef Tree::Node                              Node;
    typedef Tree::NodeMap                           NodeChildMap;

    typedef std::set<Node*>                         NodeSet;
    typedef std::set<std::string>                   StringSet;
    typedef std::list<std::string>                  StringList;
    typedef std::map<std::string, Node*>            NameNodeMap;


  public:

    TnmsTree();
    virtual ~TnmsTree();

    bool       add              ( const std::string  & name );
    bool       remove           ( const std::string  & name );
    bool       update           ( const TnmsMetric   & metric );

    bool       request          ( const std::string  & name,
                                 TreeSubscriber      * sub );

    bool       request          ( const std::string  & name,
                                  TnmsMetric         & metric );

    bool       exists           ( const std::string  & name );


    bool       subscribe        ( const std::string  & name, 
                                  TreeSubscriber     * sub );

    bool       unsubscribe      ( const std::string  & name, 
                                  TreeSubscriber     * sub );

    bool       subStructure     ( TreeSubscriber     * sub );
    bool       unsubStructure   ( TreeSubscriber     * sub );

    void       removeSubscriber ( TreeSubscriber     * sub );
    
    void       updateSubscribers();

    void       sweep            ( TnmsTree::Node     * node );
    void       sweep            ( const std::string  & name );
    void       sweep();

    size_t     size()  const;
    bool       empty() const;
    void       clear();

    void       debugDump() const;
    void       debugDump        ( const std::string  & name ) const;
    void       debugDump        ( const std::string  & name,
                                  StringList         & strlist) const;

    void       getRootNames     ( StringSet     & rootnames ) const;

    template<typename Predicate_>
    void       walkNodes        ( TnmsTree::Node     * node,
                                  Predicate_         & predicate );

  protected:

    bool       markForRemove    ( const std::string  & name );
    void       clearNodeErase   ( TnmsTree::Node     * node );

    void       resetMetric      ( TnmsMetric       & metric );


  private:

    Tree  *             _tree;
    NotifySet           _structureSubs;
    NotifySet           _allSubs;
    NotifySet           _rootSubs;

    NameNodeMap         _updates;
};

}  // namespace

#endif  //  _TNMSCORE_TNMSTREE_H_

