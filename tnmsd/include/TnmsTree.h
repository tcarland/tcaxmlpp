#ifndef _TNMSD_TNMSTREE_H_
#define _TNMSD_TNMSTREE_H_

#include <set>
#include <list>
#include <map>

#include "TnmsMetric.h"
#include "HeirarchicalStringTree.hpp"
using namespace tnmsCore;


namespace tnmsd {

class TnmsClient;


class TnmsTree {

  public:

    typedef std::set<TnmsClient*>   ClientSet;

    struct TnmsTreeNode
    {
        TnmsMetric  metric;
        ClientSet   nodeSubscribers;
        ClientSet   levelSubscribers;
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

    bool       add            ( const std::string & name );
    bool       remove         ( const std::string & name );
    bool       update         ( const TnmsMetric  & metric );

    bool       request        ( const std::string & name,
                                TnmsClient        * client );
    bool       request        ( const std::string & name,
                                TnmsMetric        & metric );

    bool       exists         ( const std::string & name );

    bool       subscribe      ( const std::string & name, 
                                TnmsClient        * client );
    bool       unsubscribe    ( const std::string & name, 
                                TnmsClient        * client );

    bool       subStructure   ( TnmsClient        * client );
    bool       unsubStructure ( TnmsClient        * client );

    void       updateClients();
    void       removeClient   ( TnmsClient        * client );

    void       sweep();
    void       sweep          ( TnmsTree::Node    * node );
    void       sweep          ( const std::string & name );


    size_t     size() const;
    void       debugDump();

  protected:

    bool       markForRemove  ( const std::string & name );
    void       clearNodeErase ( TnmsTree::Node    * node );

    void       resetMetric    ( TnmsMetric        & metric );

  private:

    Tree  *      _tree;
    ClientSet    _structureSubs;
    ClientSet    _allSubs;
    ClientSet    _rootSubs;

    NameNodeMap  _updates;
};

}  // namespace

#endif  //  _TNMSD_TNMSTREE_H_
