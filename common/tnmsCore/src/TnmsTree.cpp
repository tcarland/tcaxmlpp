#define _TNMSD_TNMSTREE_CPP_

#include "LogFacility.h"
using namespace tcanetpp;


#include "TnmsTree.h"
#include "TreeSubscriber.hpp"


namespace tnmsCore {


namespace TnmsTreeInternal {

    struct AddForwarder {

        TreeSubscriber * sub;
        bool             abort;

        AddForwarder ( TreeSubscriber * sub_ )
            : sub(sub_), abort(false)
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( sub && ! abort )
                sub->queueAdd(node);
        }
    };

    struct RemoveForwarder {
        TreeSubscriber * sub;

        RemoveForwarder ( TreeSubscriber * sub_ )
            : sub(sub_) 
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( sub )
                sub->queueRemove(node);
        }
    };

    struct MetricForwarder {
        TreeSubscriber * sub;

        MetricForwarder ( TreeSubscriber * sub_ )
            : sub(sub_)
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( sub )
                sub->queueUpdate(node);
        }
    };

    struct BreadthOrdering {
        std::list<TnmsTree::Node*>  nodes;

        void operator() ( TnmsTree::Node * node )
        {
            nodes.push_front(node);
        }
    };

    struct LeafNodes {
        std::list<TnmsTree::Node*>  nodes;

        void operator() ( TnmsTree::Node * node )
        {
            if ( node && node->getChildren().empty() )
                nodes.push_back(node);
        }
    };

    struct RemoveClient {
        TreeSubscriber * sub;

        RemoveClient ( TreeSubscriber * sub_ )
            : sub(sub_)
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( node == NULL )
                return;
            node->getValue().nodeSubscribers.erase(sub);
            node->getValue().levelSubscribers.erase(sub);
        }
    };

    struct MarkNodes {
        std::set<TreeSubscriber*>  subs;

        MarkNodes ( std::set<TreeSubscriber*> subs_ )
            : subs(subs_)
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( node == NULL ) 
                return;

            TnmsTree::TnmsTreeNode & tnode = node->getValue();
            tnode.erase = true;

            std::set<TreeSubscriber*>::iterator  cIter;
            for ( cIter = subs.begin(); cIter != subs.end(); ++cIter ) {
                TreeSubscriber * sub = (TreeSubscriber*) *cIter;
                if ( sub )
                    sub->queueRemove(node);
            }
        }
    };

    struct SweepNodes {
        std::list<TnmsTree::Node*>  nodes;

        void operator() ( TnmsTree::Node * node ) 
        {
            if ( node == NULL ) 
                return;
            if ( node->getValue().erase )
                nodes.push_back(node);
        }
    };

}  // namespace TnmsTreeInternal


using TnmsTreeInternal::AddForwarder;
using TnmsTreeInternal::RemoveForwarder;
using TnmsTreeInternal::MetricForwarder;
using TnmsTreeInternal::BreadthOrdering;
using TnmsTreeInternal::LeafNodes;
using TnmsTreeInternal::RemoveClient;
using TnmsTreeInternal::MarkNodes;
using TnmsTreeInternal::SweepNodes;



TnmsTree::TnmsTree()
    : _tree(new Tree())
{}

TnmsTree::~TnmsTree()
{
    delete _tree;
}


bool
TnmsTree::add ( const std::string & name )
{
    Node * node = _tree->find(name);

    if ( node == NULL ) {
        NodeSet  addset;

        node = _tree->insert(name, std::inserter(addset, addset.begin()));

        if ( node == NULL )
            return false;

        NodeSet::iterator nIter;

        for ( nIter = addset.begin(); nIter != addset.end(); ++nIter ) {
            NotifySet::iterator  cIter;
            std::string          addname = (*nIter)->getAbsoluteName();

            (*nIter)->getValue().metric = TnmsMetric(addname, METRIC_MESSAGE);

            for ( cIter = _structureSubs.begin(); cIter != _structureSubs.end(); ++cIter )
                (*cIter)->queueAdd(*nIter);
        }
    } else if ( node->getValue().erase ) {
        this->clearNodeErase(node);
    }

    return true;
}

bool
TnmsTree::exists ( const std::string & name )
{
    Node * node = _tree->find(name);

    if ( node == NULL )
        return false;

    return true;
}


bool
TnmsTree::remove ( const std::string & name )
{
    return this->markForRemove(name);
}


bool
TnmsTree::update ( const TnmsMetric & metric )
{
    Node * node = _tree->find(metric.getElementName());

    if ( node == NULL ) {  // implicit add

        if ( ! this->add(metric.getElementName()) )
            return false;

        node = _tree->find(metric.getElementName());
        if ( node == NULL )
            return false;
    }

    // undo erase
    if ( node->getValue().erase )
        this->clearNodeErase(node);

    TnmsMetric & m = node->getValue().metric;

    m.message_type(METRIC_MESSAGE);
    m += metric;

    _updates[m.getElementName()] = node;

    return true;
}


bool
TnmsTree::request ( const std::string & name, TreeSubscriber * sub )
{
    if ( name.compare("*") == 0 ) {  // requesting tree
        NodeChildMap  & roots = _tree->getRoots();
        NodeChildMap::iterator  nIter;

        if ( roots.empty() )
            return true;

        LogFacility::LogMessage("TnmsTree::request * from subscriber");
        //AddForwarder adds(sub);
        MetricForwarder adds(sub);

        for ( nIter = roots.begin(); nIter != roots.end(); ++nIter )
            _tree->depthFirstTraversal(nIter->second, adds);

    } else {
        Node * node = _tree->find(name);

        if ( node == NULL || sub == NULL )
            return false;

        if ( ! node->getValue().erase )
            sub->queueUpdate(node);
    }

    return true;
}


bool
TnmsTree::request ( const std::string & name, TnmsMetric & metric )
{
    Node * node = _tree->find(name);

    if ( node == NULL )
        return false;

    if ( node->getValue().erase )
        return false;

    metric = node->getValue().metric;

    return true;
}


bool
TnmsTree::subscribe ( const std::string & name, TreeSubscriber * sub )
{
    if ( name.empty() )
        return false;

    Node * node = NULL;
    std::string::size_type  len = name.length();
    char lvl = name.at(len-1);

    if ( len == 1 && lvl == '*') {
        _allSubs.insert(sub);
        _structureSubs.insert(sub);

        // trigger tree send?
        return this->request("*", sub);

    } else if ( lvl == '/' ) {
        NodeChildMap  * nodemap = NULL;
        std::string     element = name;

        element.erase(len-1, 1);

        if ( element.empty() ) {
            _rootSubs.insert(sub);
            nodemap = &_tree->getRoots();
        } else {
            node  = _tree->find(element);

            if ( node == NULL || node->getValue().erase )
                return false;

            node->getValue().levelSubscribers.insert(sub);
            nodemap = &node->getChildren();
        }

        NodeChildMap::iterator  nIter;
        for ( nIter = nodemap->begin(); nIter != nodemap->end(); ++nIter ) {
            if ( ! nIter->second->getValue().erase ) {
                //sub->queueAdd(nIter->second);
                sub->queueUpdate(nIter->second);
                //_updates[nIter->second->getValue().metric.getElementName()] = nIter->second;
            }
        }

    } else {
        node = _tree->find(name);

        if ( node == NULL )
            return false;

        node->getValue().nodeSubscribers.insert(sub);
        node->getValue().metric.message_type(METRIC_MESSAGE);
        //sub->queueAdd(node);
        sub->queueUpdate(node);
        _updates[node->getValue().metric.getElementName()] = node;
    }

    return true;
}


bool
TnmsTree::unsubscribe ( const std::string & name, TreeSubscriber * sub )
{
    if ( name.empty() )
        return false;

    std::string::size_type len = name.length();
    
    char   lvl  = name.at(len-1);
    Node * node = NULL;

    if ( len == 1 && lvl == '*' ) {
        _allSubs.erase(sub);
        _structureSubs.erase(sub);
    } else if ( lvl == '/' ) {
        std::string  element = name;

        element.erase(len-1, 1);

        if ( element.empty() ) {
            _rootSubs.erase(sub);
        } else {
            node = _tree->find(element);
            if ( node == NULL )
                return false;

            node->getValue().levelSubscribers.erase(sub);
        }
    } else {
        node = _tree->find(name);

        if ( node == NULL ) 
            return false;

        node->getValue().nodeSubscribers.erase(sub);
    }

    return true;
}


bool
TnmsTree::subStructure ( TreeSubscriber * sub )
{
    _structureSubs.insert(sub);
    return true;
}

bool
TnmsTree::unsubStructure ( TreeSubscriber * sub )
{
    _structureSubs.erase(sub);
    return true;
}


bool
TnmsTree::markForRemove ( const std::string & name )
{
    Node * node = _tree->find(name);

    if ( node == NULL )
        return false;

    NotifySet  nodesubs = node->getValue().nodeSubscribers;

    nodesubs.insert(_allSubs.begin(), _allSubs.end());
    nodesubs.insert(_structureSubs.begin(), _structureSubs.end());

    MarkNodes  marker(nodesubs);
    _tree->depthFirstTraversal(node, marker);

    return true;
}


void
TnmsTree::sweep()
{
    NodeChildMap  & roots = _tree->getRoots();
    NodeChildMap::iterator  nIter;

    if ( roots.empty() )
        return;

    for ( nIter = roots.begin(); nIter != roots.end(); ++nIter )
        this->sweep(nIter->second);

    return;
}

void
TnmsTree::sweep ( TnmsTree::Node * node )
{
    if ( node == NULL )
        return;

    Node      * nptr = NULL;
    SweepNodes  sweep;

    std::list<TnmsTree::Node*>::iterator  nIter;

    _tree->depthFirstTraversal(node, sweep);

    for ( nIter = sweep.nodes.begin(); nIter != sweep.nodes.end(); ++nIter ) {
        StringSet  removes;

        nptr = (Node*) *nIter;

        _updates.erase(nptr->getValue().metric.getElementName());
        _tree->erase(nptr, std::inserter(removes, removes.begin()));
    }

    return;
}

void
TnmsTree::sweep ( const std::string & name )
{
    Node * node = _tree->find(name);

    if ( node == NULL )
        return;

    this->sweep(node);
    return;
}

void
TnmsTree::updateSubscribers()
{
    NameNodeMap::iterator  mIter;
    NotifySet::iterator    cIter;

    for ( mIter = _updates.begin(); mIter != _updates.end(); ++mIter ) {
        Node * node   = mIter->second;
        Node * parent = node->getParent();

        if ( node == NULL )
            continue;  // really?

        NotifySet  nodesubs = node->getValue().nodeSubscribers;

        nodesubs.insert(_allSubs.begin(), _allSubs.end());

        if ( parent == NULL ) {
            nodesubs.insert(_rootSubs.begin(), _rootSubs.end());
        } else {
            NotifySet & lvlsubs = parent->getValue().levelSubscribers;
            nodesubs.insert(lvlsubs.begin(), lvlsubs.end());
        }

        for ( cIter = nodesubs.begin(); cIter != nodesubs.end(); ++cIter)
            (*cIter)->queueUpdate(node);

        // reset metric?
        this->resetMetric(node->getValue().metric);
    }
    _updates.clear();

    return;
}


void
TnmsTree::removeSubscriber ( TreeSubscriber * sub )
{
    NodeChildMap &  roots = _tree->getRoots();

    if ( roots.empty() )
        return;

    NodeChildMap::iterator  cIter;

    RemoveClient eraser(sub);

    for ( cIter = roots.begin(); cIter != roots.end(); ++cIter )
        _tree->depthFirstTraversal(cIter->second, eraser); 

    _allSubs.erase(sub);
    _structureSubs.erase(sub);
    _rootSubs.erase(sub);

    return;
}


size_t
TnmsTree::size() const
{
    return _tree->size();
}


void
TnmsTree::clearNodeErase ( TnmsTree::Node * node )
{
    NotifySet::iterator  sIter;

    TnmsTree::Node * nptr = node;

    while ( nptr ) {

        if ( ! nptr->getValue().erase )
            break;

        TnmsTree::TnmsTreeNode & tnode = nptr->getValue();
        tnode.erase = false;

        for ( sIter = _structureSubs.begin(); sIter != _structureSubs.end(); ++sIter )
            (*sIter)->queueAdd(nptr);

        nptr = nptr->getParent();
    }

    return;
}


void
TnmsTree::resetMetric ( TnmsMetric & metric )
{
    return metric.reset();
}

void
TnmsTree::getRootNames ( StringSet & rootnames ) const
{
    NodeChildMap &   roots = _tree->getRoots();
    NodeChildMap::iterator   nIter;

    if ( roots.empty() )
        return;

    for ( nIter = roots.begin(); nIter != roots.end(); ++nIter )
    {
        std::string  name = nIter->second->getValue().metric.getElementName();
        rootnames.insert(name);
    }

    return;
}

    
void
TnmsTree::debugDump() const
{
    NodeChildMap &   roots = _tree->getRoots();
    NodeChildMap::iterator    rootI;

    if ( roots.empty() )
        return;
    
    BreadthOrdering  flattenedTree;
    for ( rootI = roots.begin(); rootI != roots.end(); ++rootI )
        _tree->depthFirstTraversal( rootI->second, flattenedTree );

    std::list<TnmsTree::Node*>::iterator nIter;

    LogFacility::LogMessage("TnmsTree::debugDump(): " + StringUtils::toString(_tree->size()));

    for ( nIter = flattenedTree.nodes.begin(); nIter != flattenedTree.nodes.end(); ++nIter ) {
        LogFacility::Message  msg;
        msg << "    Node: " << "  " << (*nIter)->getValue().metric.getElementName();
        if ( (*nIter)->getValue().erase )
            msg << " <ERASED>";
        LogFacility::LogMessage(msg.str());
    }

    return;
}

void
TnmsTree::debugDump ( const std::string & name ) const
{
    Node * node = _tree->find(name);

    if ( node == NULL )
        return;

    BreadthOrdering  flattenedTree;
    _tree->depthFirstTraversal(node, flattenedTree);

    std::list<TnmsTree::Node*>::iterator nIter;
    LogFacility::LogMessage("TnmsTree::debugDump() for " + name);
    
    for ( nIter = flattenedTree.nodes.begin(); nIter != flattenedTree.nodes.end(); ++nIter ) {
        LogFacility::Message  msg;
        msg << "    Node: " << "  " << (*nIter)->getValue().metric.getElementName();
        if ( (*nIter)->getValue().erase )
            msg << " <ERASED>";
        LogFacility::LogMessage(msg.str());
    }

    return;
}


} // namespace

