#define _TNMSD_TNMSTREE_CPP_



#include "TnmsTree.h"


namespace tnmsd {



namespace TnmsTreeInternal {

    struct AddForwarder {

        TnmsClient * client;
        bool         abort;

        AddForwarder ( TnmsClient * client_ )
            : client(client_), abort(false)
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( client && ! abort )
                client->queueAdd(node);
        }
    };

    struct RemoveForwarder {
        TnmsClient * client;

        RemoveForwarder ( TnmsClient * client_ )
            : client(client_) 
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( client )
                client->queueRemove(node);
        }
    };

    struct MetricForwarder {
        TnmsClient * client;

        MetricForwarder ( TnmsClient * client_ )
            : client(client_)
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( client )
                client->queueUpdate(node);
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
        TnmsClient * client;

        RemoveClient ( TnmsClient * client_ )
            : client(client_)
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( node == NULL )
                return;
            node->getValue().nodeSubscribers.erase(client);
            node->getValue().lvlSubscribers.erase(client);
        }
    };

    struct MarkNodes {
        std::set<TnmsClient*> subs;

        MarkNodes ( std::set<TnmsClient*> subs_ )
            : subs(subs_)
        {}

        void operator() ( TnmsTree::Node * node )
        {
            if ( node == NULL ) 
                return;
            std::set<TnmsClient*>::iterator  cIter;
            for ( cIter = subs.begin(); cIter != subs.end(); ++cIter ) {
                TnmsClient * client = (TnmsClient*) *cIter;
                if ( client )
                    client->queueRemove(node);
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

}  // namespace

using namespace TnmsTreeInternal::AddForwarder;
using namespace TnmsTreeInternal::RemoveForwarder;
using namespace TnmsTreeInternal::MetricForwarder;
using namespace TnmsTreeInternal::BreadthOrdering;
using namespace TnmsTreeInternal::LeafNodes;
using namespace TnmsTreeInternal::RemoveClient;
using namespace TnmsTreeInternal::MarkNodes;
using namespace TnmsTreeInternal::SweepNodes;



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
            ClientSet::iterator  cIter;
            std::string          addname;

            (*cIter)->getValue().metric = TnmsMetric(addname, ADD_MESSAGE);

            for ( cIter = _structureSubs.begin(); cIter != _structureSubs.end(); ++cIter )
                (*cIter)->QueueAddMessage(*nIter);
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

    if ( node == NULL ) {

        if ( ! this->add(metric.getElementName()) )
            return false;

        node = _tree->find(metric.getElementName());
        if ( node == NULL )
            return false;
    }

    // reset
    if ( node->getValue().erase )
        this->clearNodeErase(node);

    node->getValue().metric.message_type(METRIC_MESSAGE);
    updatedNodes[metric.getElementName()] = node;

    // rollups?

    return true;
}


bool
TnmsTree::request ( const std::string & name, TnmsClient * client )
{
    Node * node = _tree->find(name);

    if ( node == NULL || client == NULL )
        return false;

    if ( ! node->getValue().erase )
        client->queueUpdate(node->getValue().metric);

    return true;
}


bool
TnmsTree::request ( const std:;string & name, TnmsMetric & metric )
{
    Node * node = _tree->find(name);

    if ( node == NULL || client == NULL )
        return false;

    if ( node->getValue().erase )
        return false;

    metric = node->getValue().metric;

    return true;
}


bool
TnmsTree::subscribe ( const std::string & name, TnmsClient * client )
{
    if ( name.empty() )
        return false;

    std::string::size_type  len = name.length();
    char lvl = name.at(len-1);

    if ( len == 1 && lvl == '*') {
        _allSubs.insert(client);
        _structureSubs.insert(client);

        // trigger tree send?

    } else if ( lvl == '/' ) {
        NodeChildMap  * nodemap = NULL;
        std::string     element = name;

        element.erase(len-1, 1);

        if ( element.empty() ) {
            _rootSubs.insert(client);
            nodemap = &tree->getRoots();
        } else {
            node  = _tree->find(element);

            if ( node == NULL || node->getValue().erase )
                return false;

            node->getValue().levelSubscribers.insert(client);
            nodemap = &node->getChildren();
        }

        NodeChildMap::iterator  nIter;

        for ( nIter = nodemap->begin(); nIter != nodemap.end(); ++nIter ) {
            if ( ! nIter->second->getValue().erase )
                client->queueUpdate(nIter->second);
        }
    } else {
        node = _tree->find(name);

        if ( node == NULL )
            return false;

        node->getValue().nodeSubscribers.insert(client);
        node->getValue().metric.message_type(METRIC_MESSAGE);

        client->queueUpdate(node);
    }

    return true;
}


bool
TnmsTree::unsubscribe ( const std::string & name, TnmsClient * client )
{
    if ( name.empty() )
        return false;

    std::string::size_type len = name.length();
    
    char   lvl  = name.at(len-1);
    Node * node = NULL;

    if ( len == 1 && lvl == '*' ) {
        _allSubs.erase(client);
        _structureSubs.erase(client);
    } else if ( lvl = '/' ) {
        std::string  element = name;

        element.erase(len-1, 1);

        if ( element.empty() ) {
            _rootSubs.erase(client);
        } else {
            node = _tree->find(element);
            if ( node == NULL )
                return false;

            node->getValue().levelSubscribers.erase(client);
        }
    } else {
        node = _tree->find(name);

        if ( node == NULL ) 
            return false;

        node->getValue().nodeSubscribers.erase(client);
    }

    return true;
}


bool
TnmsTree::subStructure ( TnmsClient * client )
{
    _structureSubs.insert(client);
    return true;
}

bool
TnmsTree::unsubStructure ( TnmsClient * client )
{
    _structureSubs.erase(client);
    return true;
}


bool
TnmsTree::markForRemove ( const std::string & name )
{
    Node * node = _tree->find(name);

    if ( node == NULL )
        return false;

    ClientSet  nodesubs = node->getValue().nodeSubscribers;

    nodesubs.insert(_allSubs.begin(), _allSubs.end());
    nodesubs.insert(_structureSubs.begin(); _structureSubs.end());

    MarkNodes  marker(nodesubs);
    _tree->depthFirstTraversal(node, marker);

    return true;
}


void
TnmsTree::sweep()
{
    NodeChildMap  & roots = tree->getRoots();
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

    Node * nptr = NULL;

    std::list<TnmsTree::Node*>::iterator  nIter;

    SweepNodes  sweep;

    _tree->depthFirstTraversal(node, sweeper);

    for ( nIter = sweep.nodes.begin(); nIter != sweep.nodes.end(); ++nIter ) {
        StringList  removes;

        nptr = (Node*) *nIter;

        _updates.erase(nptr->getValue().metric.getElementName());
        _tree->erase(nptr, std::inserter(removes.begin()));
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


size_t
TnmsTree::size() const
{
    return _tree->size();
}


void
TnmsTree:clearNodeErase ( TnmsTree::Node * node )
{

}


void
TnmsTree::resetMetric   ( TnmsMetric & metric )
{
}


} // namespace
