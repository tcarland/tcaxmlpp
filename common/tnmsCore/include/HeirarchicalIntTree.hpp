#ifndef _TNMSCORE_HEIRARCHICALINTTREE_HPP_
#define _TNMSCORE_HEIRARCHICALINTTREE_HPP_

#include <string>
#include <map>
#include <list>
#include <vector>
#include <stdexcept>

#include "TnmsOid.h"


namespace tnmsCore {


template < typename ValueType >
class HeirarchicalIntTreeNode {
  public:

    typedef HeirarchicalIntTreeNode<ValueType>      TreeNode;
    typedef std::map<uint16_t, TreeNode*>           TreeNodeMap;
    typedef typename TreeNodeMap::iterator          TreeNodeMapIter;

  public:

    HeirarchicalIntTreeNode ( TnmsOid  * nodeOid,
    	                      TreeNode * parent );

    virtual ~HeirarchicalIntTreeNode();

    uint16_t             getName() const;
    const TnmsOid&       getAbsoluteName() const;

    ValueType&           getValue();
    const ValueType&     getValue() const;

    TreeNode*            getParent();
    const TreeNode*      getParent() const;

    TreeNodeMap&         getChildren();
    const TreeNodeMap&   getChildren() const;

  private:

    ValueType           _value;
    TnmsOid  *          _nodeOid;
    TreeNode *          _parent;
    char                _delim;
    TreeNodeMap         _children;
};


template < typename ValueType >
class HeirarchicalIntTree {
  public:

    typedef HeirarchicalIntTreeNode<ValueType>      Node;
    typedef typename Node::TreeNodeMap              NodeMap;
    typedef typename Node::TreeNodeMapIter          NodeMapIter;

    typedef std::vector<uint16_t>                   IntList;
    typedef typename IntList::iterator              IntListIter;
    typedef std::list<Node*>                        BranchNodeList;
    typedef typename BranchNodeList::iterator       BranchNodeListIter;

  public:

    HeirarchicalIntTree ( char delimiter = '.' );
    HeirarchicalIntTree ( const HeirarchicalIntTree & tree );

    virtual ~HeirarchicalIntTree();


    HeirarchicalIntTree&  operator= ( const HeirarchicalIntTree & tree );


    char            getDelimiter() const;

    NodeMap&        getRoots();
    const NodeMap&  getRoots() const;

    int             size() const;

    Node*           find   ( const TnmsOid & oid );

    template<typename OutputIterator_>
    Node*           insert ( const TnmsOid & oid,
                             OutputIterator_ outIter )
        throw ( std::runtime_error );

    template<typename OutputIterator_>
    bool            erase  ( const TnmsOid & oid,
                             OutputIterator_ outIter );

    template<typename OutputIterator_>
    bool            erase  ( Node * node,
                             OutputIterator_ outIter );

    bool            erase  ( Node * node );

    void            clear();

    template<typename BranchIterator_, typename OutputIterator_>
    bool            nodesFromBranches   ( BranchIterator_ beginIter,
                                          BranchIterator_ endIter,
                                          OutputIterator_ outIter );

    template<typename Predicate_>
    void            depthFirstTraversal ( Node * node,
                                          Predicate_ & predicate );

  public:

    struct BreadthOrderingFunctor
    {
        BranchNodeList  nodes;
        void operator() ( Node * node ) { nodes.push_front(node); }
    };

    struct DepthOrderingFunctor
    {
        BranchNodeList  nodes;
        void operator() ( Node * node ) { nodes.push_back(node); }
    };

  private:

    NodeMap             _roots;
    char                _delim;
    int                 _size;
};

}  // namespace

#include "../src/HeirarchicalIntTree.cpp"

#endif // _TNMSCORE_HEIRARCHICALINTTREE_HPP_

