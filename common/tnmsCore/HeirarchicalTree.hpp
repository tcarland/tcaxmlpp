#ifndef _HEIRARCHICALTREE_HPP_
#define _HEIRARCHICALTREE_HPP_

#include <string>
#include <map>
#include <list>
#include <vector>
#include <stdexcept>


namespace tnmscore {


class TreeException : public std::runtime_error {
public:
    explicit TreeException ( const std::string & errorstr )
        : std::runtime_error(errorstr)
    {}
    virtual ~TreeException() throw() {}
    virtual std::string toString() const { return this->what(); }
};


template < typename ValueType >
class HeirarchicalTreeNode {
  public:

    typedef HeirarchicalTreeNode<ValueType>   TreeNode;
    typedef std::map<std::string, TreeNode*>  TreeNodeMap;
    typedef typename TreeNodeMap::iterator    TreeNodeMapIter;

  public:

    HeirarchicalTreeNode ( const std::string & nodeName, TreeNode * parent,
                           char delimiter = '/' );

    virtual ~HeirarchicalTreeNode();

    const std::string&  getName() const;
    std::string         getAbsoluteName() const;

    ValueType&          getValue();
    const ValueType&    getValue() const;

    TreeNode*           getParent();
    const TreeNode*     getParent() const;

    TreeNodeMap&        getChildren();
    const TreeNodeMap&  getChildren() const;

  private:

    ValueType           _value;
    std::string         _nodeName;
    TreeNode *          _parent;
    char                _delim;
    TreeNodeMap         _children;

};


template < typename ValueType >
class HeirarchicalTree {
  public:

    typedef HeirarchicalTreeNode<ValueType>   Node;
    typedef typename Node::TreeNodeMap        NodeMap;
    typedef typename Node::TreeNodeMapIter    NodeMapIter;

    typedef std::vector<std::string>          StringList;
    typedef typename StringList::iterator     StringListIter;
    typedef std::list<Node*>                  BranchNodeList;
    typedef typename BranchNodeList::iterator BranchNodeListIter;

  public:

    HeirarchicalTree ( char delimiter = '/' );
    HeirarchicalTree ( const HeirarchicalTree & tree );

    virtual ~HeirarchicalTree();


    HeirarchicalTree&  operator= ( const HeirarchicalTree & tree );


    char           getDelimiter() const;

    NodeMap&       getRoots();
    const NodeMap& getRoots() const;

    int            size() const;

    Node*          find   ( const std::string & absoluteName );

    template<typename OutputIterator_>
    Node*          insert ( const std::string & absoluteName,
                            OutputIterator_ outIter )
        throw ( TreeException );
    
    template<typename OutputIterator_>
    bool           erase  ( const std::string & absoluteName,
                            OutputIterator_ outIter );

    template<typename OutputIterator_>
    bool           erase  ( Node * node,
                            OutputIterator_ outIter );

    void           clear();

    template<typename BranchIterator_, typename OutputIterator_>
    bool           nodesFromBranches   ( BranchIterator_ bIter,
                                         BranchIterator_ end,
                                         OutputIterator_ outIter );

    template<typename Predicate_>
    void           depthFirstTraversal ( Node * node, 
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

#include "HeirarchicalTree.cpp"


#endif // _HEIRARCHICALTREE_HPP_

