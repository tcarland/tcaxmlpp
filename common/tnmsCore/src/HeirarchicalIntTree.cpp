#ifndef _HEIRARCHICALINTTREE_CPP_
#define _HEIRARCHICALINTTREE_CPP_

#include "StringUtils.h"
using namespace tcanetpp;


namespace tnmsCore {


template<typename ValueType>
HeirarchicalIntTreeNode<ValueType>::HeirarchicalIntTreeNode ( TnmsOid  * nodeOid,
                                                              TreeNode * parent )
    : _nodeOid(nodeOid),
      _parent(parent)
{}


template<typename ValueType>
HeirarchicalIntTreeNode<ValueType>::~HeirarchicalIntTreeNode()
{
    if ( _nodeOid )
        delete _nodeOid;
}

template<typename ValueType>
uint16_t
HeirarchicalIntTreeNode<ValueType>::getName() const
{
    return _nodeOid->lastValue();
}


template<typename ValueType>
const TnmsOid&
HeirarchicalIntTreeNode<ValueType>::getAbsoluteName() const
{
    return *_nodeOid;
}


template<typename ValueType>
ValueType&
HeirarchicalIntTreeNode<ValueType>::getValue()
{
    return _value;
}


template<typename ValueType>
const ValueType&
HeirarchicalIntTreeNode<ValueType>::getValue() const
{
    return _value;
}


template<typename ValueType>
typename HeirarchicalIntTreeNode<ValueType>::TreeNode*
HeirarchicalIntTreeNode<ValueType>::getParent()
{
    return _parent;
}


template<typename ValueType>
const typename HeirarchicalIntTreeNode<ValueType>::TreeNode*
HeirarchicalIntTreeNode<ValueType>::getParent() const
{
    return _parent;
}


template<typename ValueType>
typename HeirarchicalIntTreeNode<ValueType>::TreeNodeMap&
HeirarchicalIntTreeNode<ValueType>::getChildren()
{
    return _children;
}


template<typename ValueType>
const typename HeirarchicalIntTreeNode<ValueType>::TreeNodeMap&
HeirarchicalIntTreeNode<ValueType>::getChildren() const
{
    return _children;
}


//-------------------------------------------------------------//


template<typename ValueType>
HeirarchicalIntTree<ValueType>::HeirarchicalIntTree ( char delimiter )
    : _delim(delimiter),
      _size(0)
{}


template<typename ValueType>
HeirarchicalIntTree<ValueType>::HeirarchicalIntTree ( const HeirarchicalIntTree & tree )
{}


template<typename ValueType>
HeirarchicalIntTree<ValueType>::~HeirarchicalIntTree()
{
    this->clear();
}


template<typename ValueType>
typename HeirarchicalIntTree<ValueType>::HeirarchicalIntTree&
HeirarchicalIntTree<ValueType>::operator= ( const HeirarchicalIntTree & tree )
{}


template<typename ValueType>
char
HeirarchicalIntTree<ValueType>::getDelimiter() const
{
    return _delim;
}


template<typename ValueType>
typename HeirarchicalIntTree<ValueType>::NodeMap&
HeirarchicalIntTree<ValueType>::getRoots()
{
    return _roots;
}


template<typename ValueType>
const typename HeirarchicalIntTree<ValueType>::NodeMap&
HeirarchicalIntTree<ValueType>::getRoots() const
{
    return _roots;
}


template<typename ValueType>
int
HeirarchicalIntTree<ValueType>::size() const
{
    return _size;
}


template<typename ValueType>
typename HeirarchicalIntTree<ValueType>::Node*
HeirarchicalIntTree<ValueType>::find ( const TnmsOid & oid )
{
    BranchNodeList  branches;

    this->nodesFromBranches(oid.begin(), oid.end(),
                            std::back_inserter(branches));

    if ( branches.empty() || branches.size() != oid.size() )
        return NULL;

    return branches.back();
}


template<typename ValueType>
template<typename OutputIterator_>
typename HeirarchicalIntTree<ValueType>::Node*
HeirarchicalIntTree<ValueType>::insert ( const TnmsOid   & oid,
                                         OutputIterator_   outIter )
    throw ( std::runtime_error )
{
    BranchNodeList  branches;

    if ( oid.empty() )
        return NULL;

    if ( this->nodesFromBranches(oid.begin(), oid.end(),
                                 std::back_inserter(branches)) )
        return NULL;

    NodeMap * children = &_roots;
    Node    * parent   = NULL;
    Node    * node     = NULL;
    TnmsOid * noid     = NULL;

    if ( ! branches.empty() ) {
        children = &branches.back()->getChildren();
        parent   = branches.back();
    }

    OidList::size_type  bi;
    NodeMapIter         nIter;

    for ( bi = branches.size(); bi < oid.size(); ++bi ) {
        std::pair<NodeMapIter, bool> insertR = children->insert(typename NodeMap::value_type(oid[bi], 0));

        if ( ! insertR.second )
            throw std::runtime_error("insert failed on " + oid.toString());

        nIter = insertR.first; // map in elements
        noid  = TnmsOid::OidFromOidIndex(oid, bi);
        node  = new Node(noid, parent);

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
HeirarchicalIntTree<ValueType>::erase ( const TnmsOid  & oid,
                                        OutputIterator_  outIter )
{
    Node * node = this->find(oid);
    return this->erase(node, outIter);
}


template<typename ValueType>
template<typename OutputIterator_>
bool
HeirarchicalIntTree<ValueType>::erase ( Node  * node,
                                        OutputIterator_ outIter )
{
    if ( node == NULL )
        return false;

    DepthOrderingFunctor   depthfirst;
    uint16_t               oidval     = node->getName();
    NodeMap              * pChildList = &_roots;

    if ( node->getParent() != NULL )
        pChildList = &node->getParent()->getChildren();

    this->depthFirstTraversal(node, depthfirst);
    _size -= depthfirst.nodes.size();

    BranchNodeList      & branches = depthfirst.nodes;
    BranchNodeListIter    bIter;

    for ( bIter = branches.begin(); bIter != branches.end(); ++bIter ) {
        *outIter++ = (*bIter)->getAbsoluteName();
        delete *bIter;
    }

    pChildList->erase(oidval);

    return true;
}

template<typename ValueType>
bool
HeirarchicalIntTree<ValueType>::erase ( Node * node )
{
    if ( node == NULL )
        return false;

    DepthOrderingFunctor   depthfirst;
    uint16_t               oidval     = node->getName();
    NodeMap              * pChildList = &_roots;

    if ( node->getParent() != NULL )
        pChildList = &node->getParent()->getChildren();

    this->depthFirstTraversal(node, depthfirst);
    _size -= depthfirst.nodes.size();

    BranchNodeList      & branches = depthfirst.nodes;
    BranchNodeListIter    bIter;

    for ( bIter = branches.begin(); bIter != branches.end(); ++bIter )
        delete *bIter;

    pChildList->erase(oidval);

    return true;
}

template<typename ValueType>
void
HeirarchicalIntTree<ValueType>::clear()
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
HeirarchicalIntTree<ValueType>::nodesFromBranches ( BranchIterator_ beginIter,
                                                    BranchIterator_ endIter,
                                                    OutputIterator_ outIter )
{
    NodeMap *  children = &_roots;

    for ( ; beginIter != endIter; ++beginIter ) {
        NodeMapIter  nIter;

        if ( (nIter = children->find(*beginIter)) == children->end() )
            return false;

        *outIter++ = nIter->second;
        children   = &nIter->second->getChildren();
    }

    return true;
}


template<typename ValueType>
template<typename Predicate_>
void
HeirarchicalIntTree<ValueType>::depthFirstTraversal ( Node * node, Predicate_ & predicate )
{
    NodeMap & children = node->getChildren();

    NodeMapIter  nIter;
    for ( nIter = children.begin(); nIter != children.end(); ++nIter )
        this->depthFirstTraversal(nIter->second, predicate);

    predicate(node);

    return;
}


} // namespace

#endif

