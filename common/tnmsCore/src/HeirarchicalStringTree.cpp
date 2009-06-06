#ifndef _HEIRARCHICALSTRINGTREE_CPP_
#define _HEIRARCHICALSTRINGTREE_CPP_


#include "StringUtils.h"
using namespace tcanetpp;


namespace tnmsCore {


//-------------------------------------------------------------//
//  HeirarchicalStringTreeNode


template<typename ValueType>
HeirarchicalStringTreeNode<ValueType>::HeirarchicalStringTreeNode ( const std::string & nodeName,
                                                                    TreeNode * parent,
                                                                    char delimiter )
    : _nodeName(nodeName),
      _parent(parent),
      _delim(delimiter)
{}


template<typename ValueType>
HeirarchicalStringTreeNode<ValueType>::~HeirarchicalStringTreeNode()
{}


template<typename ValueType>
const std::string&
HeirarchicalStringTreeNode<ValueType>::getName() const
{
    return _nodeName;
}


template<typename ValueType>
std::string
HeirarchicalStringTreeNode<ValueType>::getAbsoluteName() const
{
    std::string  absname = _nodeName;
    TreeNode   * next    = _parent;

    while ( next != NULL ) {
        absname.insert(absname.begin(), _delim);
        absname.insert(0, next->getName());
        next = next->getParent();
    }

    return absname;
}


template<typename ValueType>
ValueType&
HeirarchicalStringTreeNode<ValueType>::getValue()
{
    return _value;
}


template<typename ValueType>
const ValueType&
HeirarchicalStringTreeNode<ValueType>::getValue() const
{
    return _value;
}


template<typename ValueType>
typename HeirarchicalStringTreeNode<ValueType>::TreeNode*
HeirarchicalStringTreeNode<ValueType>::getParent()
{
    return _parent;
}


template<typename ValueType>
const typename HeirarchicalStringTreeNode<ValueType>::TreeNode*
HeirarchicalStringTreeNode<ValueType>::getParent() const
{
    return _parent;
}


template<typename ValueType>
typename HeirarchicalStringTreeNode<ValueType>::TreeNodeMap&
HeirarchicalStringTreeNode<ValueType>::getChildren()
{
    return _children;
}


template<typename ValueType>
const typename HeirarchicalStringTreeNode<ValueType>::TreeNodeMap&
HeirarchicalStringTreeNode<ValueType>::getChildren() const
{
    return _children;
}

//-------------------------------------------------------------//
//  HeirarchicalStringTree


template<typename ValueType>
HeirarchicalStringTree<ValueType>::HeirarchicalStringTree ( char delimiter )
    : _delim(delimiter),
      _size(0)
{}


template<typename ValueType>
HeirarchicalStringTree<ValueType>::HeirarchicalStringTree ( const HeirarchicalStringTree & tree )
{}


template<typename ValueType>
HeirarchicalStringTree<ValueType>::~HeirarchicalStringTree()
{
    this->clear();
}


template<typename ValueType>
typename HeirarchicalStringTree<ValueType>::HeirarchicalStringTree&
HeirarchicalStringTree<ValueType>::operator= ( const HeirarchicalStringTree & tree )
{}


template<typename ValueType>
char
HeirarchicalStringTree<ValueType>::getDelimiter() const
{
    return _delim;
}


template<typename ValueType>
typename HeirarchicalStringTree<ValueType>::NodeMap&
HeirarchicalStringTree<ValueType>::getRoots()
{
    return _roots;
}


template<typename ValueType>
const typename HeirarchicalStringTree<ValueType>::NodeMap&
HeirarchicalStringTree<ValueType>::getRoots() const
{
    return _roots;
}


template<typename ValueType>
int
HeirarchicalStringTree<ValueType>::size() const
{
    return _size;
}


template<typename ValueType>
typename HeirarchicalStringTree<ValueType>::Node*
HeirarchicalStringTree<ValueType>::find ( const std::string & absoluteName )
{
    StringList      branchNames;
    BranchNodeList  branches;

    StringUtils::split(absoluteName, _delim, std::back_inserter(branchNames));

    this->nodesFromBranches(branchNames.begin(), branchNames.end(),
                          std::back_inserter(branches));

    if ( branches.empty() || branches.size() != branchNames.size() )
        return NULL;

    return branches.back();
}


template<typename ValueType>
template<typename OutputIterator_>
typename HeirarchicalStringTree<ValueType>::Node*
HeirarchicalStringTree<ValueType>::insert ( const std::string & absoluteName,
                                            OutputIterator_     outIter )
    throw ( std::runtime_error )
{
    StringList      branchNames;
    BranchNodeList  branches;

    StringUtils::split(absoluteName, _delim, std::back_inserter(branchNames));

    if ( branchNames.empty() )
        return NULL;

    if ( this->nodesFromBranches(branchNames.begin(), branchNames.end(),
                                 std::back_inserter(branches)) )
        return NULL;

    NodeMap * children = &_roots;
    Node    * parent   = NULL;
    Node    * node     = NULL;

    if ( ! branches.empty() ) {
        children = &branches.back()->getChildren();
        parent   = branches.back();
    }

    StringList::size_type  bi;
    NodeMapIter      nIter;

    for ( bi = branches.size(); bi < branchNames.size(); ++bi ) {
        std::pair<NodeMapIter, bool> insertR = children->insert(typename NodeMap::value_type(branchNames[bi], 0));

        if ( ! insertR.second )
            throw std::runtime_error("insert failed on " + absoluteName);

        nIter = insertR.first;
        node  = new Node(nIter->first, parent);

        nIter->second = node;
        *outIter++    = node;
        ++_size;

        parent   = node;
        children = &node->getChildren();
    }

    return node;
}


template<typename ValueType>
template<typename OutputIterator_>
bool
HeirarchicalStringTree<ValueType>::erase ( const std::string & absoluteName,
                                           OutputIterator_     outIter )
{
    Node * node = this->find(absoluteName);
    return( this->erase(node, outIter) );
}


template<typename ValueType>
template<typename OutputIterator_>
bool
HeirarchicalStringTree<ValueType>::erase ( Node  * node,
                                           OutputIterator_ outIter )
{
    if ( node == NULL )
        return false;

    DepthOrderingFunctor   depthfirst;
    const std::string      name       = node->getName();
    NodeMap              * pChildList = &_roots;

    if ( node->getParent() != NULL )
        pChildList = &node->getParent()->getChildren();


    this->depthFirstTraversal(node, depthfirst);
    _size -= depthfirst.nodes.size();

    BranchNodeList & branches = depthfirst.nodes;
    BranchNodeListIter bIter;

    for ( bIter = branches.begin(); bIter != branches.end(); ++bIter ) {
        *outIter++ = (*bIter)->getAbsoluteName();
        delete *bIter;
    }

    pChildList->erase(name);

    return true;
}


template<typename ValueType>
void
HeirarchicalStringTree<ValueType>::clear()
{
    NodeMapIter  nIter;

    for ( nIter = _roots.begin(); nIter != _roots.end(); ++nIter ) {
        DepthOrderingFunctor  depthfirst;
        BranchNodeList      & branches = depthfirst.nodes;
        BranchNodeListIter    bIter;

        this->depthFirstTraversal(nIter->second, depthfirst);
        _size -= depthfirst.nodes.size();

        for ( bIter = branches.begin(); bIter != branches.end(); ++bIter )
            delete *bIter;
    }
    _roots.clear();

    return;
}


template<typename ValueType>
template<typename BranchIterator_, typename OutputIterator_>
bool
HeirarchicalStringTree<ValueType>::nodesFromBranches ( BranchIterator_ bIter,
                                                       BranchIterator_ endIter,
                                                       OutputIterator_ outIter )
{
    NodeMap *  children = &_roots;

    for ( ; bIter != endIter; ++bIter ) {
        NodeMapIter nIter;

        if ( (nIter = children->find(*bIter)) == children->end() )
            return false;

        *outIter++ = nIter->second;
        children   = &nIter->second->getChildren();
    }

    return true;
}


template<typename ValueType>
template<typename Predicate_>
void
HeirarchicalStringTree<ValueType>::depthFirstTraversal ( Node * node, Predicate_ & predicate )
{
    if ( node == NULL )
        return;

    NodeMap & children = node->getChildren();

    NodeMapIter  nIter;
    for ( nIter = children.begin(); nIter != children.end(); ++nIter )
        this->depthFirstTraversal(nIter->second, predicate);

    predicate(node);

    return;
}


} // namespace

#endif

