#ifndef _TNMSCORE_HEIRARCHICALSTRINGTREE_HPP_
#define _TNMSCORE_HEIRARCHICALSTRINGTREE_HPP_

#include <string>
#include <map>
#include <list>
#include <vector>
#include <stdexcept>


namespace tnmsCore {


template < typename ValueType >
class HeirarchicalStringTreeNode {
  public:

    typedef HeirarchicalStringTreeNode<ValueType>   TreeNode;
    typedef std::map<std::string, TreeNode*>        TreeNodeMap;
    typedef typename TreeNodeMap::iterator          TreeNodeMapIter;

  public:

    HeirarchicalStringTreeNode ( const std::string & nodeName,
                                 TreeNode          * parent,
                                 char                delimiter = '/' );

    virtual ~HeirarchicalStringTreeNode();


    const std::string&   getName() const;
    std::string          getAbsoluteName() const;

    ValueType&           getValue();
    const ValueType&     getValue() const;

    TreeNode*            getParent();
    const TreeNode*      getParent() const;

    TreeNodeMap&         getChildren();
    const TreeNodeMap&   getChildren() const;

  private:

    ValueType           _value;
    std::string         _nodeName;
    TreeNode *          _parent;
    char                _delim;
    TreeNodeMap         _children;

};


template < typename ValueType >
class HeirarchicalStringTree {

  public:

    typedef HeirarchicalStringTreeNode<ValueType>   Node;
    typedef typename Node::TreeNodeMap              NodeMap;
    typedef typename Node::TreeNodeMapIter          NodeMapIter;

    typedef std::vector<std::string>                StringList;
    typedef typename StringList::iterator           StringListIter;
    typedef std::list<Node*>                        BranchNodeList;
    typedef typename BranchNodeList::iterator       BranchNodeListIter;

  public:

    HeirarchicalStringTree ( char delimiter = '/' );
    HeirarchicalStringTree ( const HeirarchicalStringTree & tree );

    virtual ~HeirarchicalStringTree();


    HeirarchicalStringTree&  operator= ( const HeirarchicalStringTree & tree );


    char            getDelimiter() const;
    NodeMap&        getRoots();
    const NodeMap&  getRoots() const;


    Node*           find   ( const std::string & absoluteName );


    template<typename OutputIterator_>
    Node*           insert ( const std::string & absoluteName,
                             OutputIterator_ outIter )
        throw ( std::runtime_error );


    template<typename OutputIterator_>
    bool            erase  ( const std::string & absoluteName,
                             OutputIterator_ outIter );

    template<typename OutputIterator_>
    bool            erase  ( Node * node,
                             OutputIterator_ outIter );

    int             size() const;
    void            clear();


    template<typename Predicate_>
    void            depthFirstTraversal ( Node       * node,
                                          Predicate_ & predicate );


    template<typename BranchIterator_, typename OutputIterator_>
    bool            branchToNodes   ( BranchIterator_  begin,
                                      BranchIterator_  end,
                                      OutputIterator_  outIter );

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

#include "../src/HeirarchicalStringTree.cpp"


#endif // _TNMSCORE_HEIRARCHICALSTRINGTREE_HPP_

