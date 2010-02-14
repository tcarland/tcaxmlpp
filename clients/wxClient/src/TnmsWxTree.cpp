#define _TNMSWXTREE_CPP_

#include <iostream> 

#include "TnmsWxTree.h"
#include "ClientSubscriber.h"

#include "LogFacility.h"
using namespace tcanetpp;


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------



// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

TnmsWxTree::TnmsWxTree()
    : _treeCtrl(NULL)
{
    this->Init();
}

TnmsWxTree::TnmsWxTree ( wxWindow        * parent,
                         const wxWindowID  id,
                         const wxString  & name,
                         const wxPoint   & pos,
                         const wxSize    & size,
                         long              style )
{
    this->Init();
    this->Create(parent, id, name, pos, size, style);
}


TnmsWxTree::~TnmsWxTree()
{
    if ( _treeCtrl )
        delete _treeCtrl;
    delete _visible;
}

// ----------------------------------------------------------------------

void
TnmsWxTree::Init()
{
    _rootId  = 0;
    _visible = new TnmsTree();
}


bool
TnmsWxTree::Create  ( wxWindow        * parent,
                      const wxWindowID  id,
                      const wxString  & name,
                      const wxPoint   & pos,
                      const wxSize    & size,
                      long              style )
{
    if ( ! wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    long treeStyle  = wxTR_HAS_BUTTONS;
    treeStyle      |= wxTR_HIDE_ROOT;
    treeStyle      |= style;

    _treeCtrl       = new wxTreeCtrl(parent, TNMS_ID_WXTREE, pos, size, treeStyle);

    wxString   rootName = wxT("/");
    TreeItem * rootData = new TreeItem("/");

    _rootId = _treeCtrl->AddRoot(rootName, -1, -1, rootData);
    _treeCtrl->SetItemHasChildren(_rootId);
    
    _treeCtrl->Expand(_rootId);
    SetInitialSize(size);
    DoResize();

    Connect(TNMS_ID_TREE, wxEVT_SIZE, wxSizeEventHandler(TnmsWxTree::OnSize));

    return true;
}

// ----------------------------------------------------------------------

TreeItem*
TnmsWxTree::GetItemData ( wxTreeItemId  id )
{
    return( (TreeItem*) _treeCtrl->GetItemData(id) );
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void
TnmsWxTree::OnSize ( wxSizeEvent & WXUNUSED(event) )
{
    this->DoResize();
}

// ----------------------------------------------------------------------

void
TnmsWxTree::OnSelect ( wxTreeEvent & event )
{
    //wxTreeItemId   id   = event.GetItem();
    //TreeItem     * data = (TreeItem*) _treeCtrl->GetItemData(id);
    //LogFacility::LogMessage("OnSelect " + data->absoluteName);
    return;
}

// ----------------------------------------------------------------------

void
TnmsWxTree::OnDelete ( wxTreeEvent & event )
{
    //LogFacility::LogMessage("TnmsWxTree::OnDelete ");
}

// ----------------------------------------------------------------------

void
TnmsWxTree::DoResize()
{
    wxSize sz = GetClientSize();
    _treeCtrl->SetSize(0, 0, sz.x, sz.y);
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

TnmsMetric
TnmsWxTree::GetItemMetric ( wxTreeItemId & id )
{
    TnmsMetric   metric;
    TreeItem   * data = this->GetItemData(id);
    
    _visible->request(data->absoluteName, metric);

    return metric;
}

// ----------------------------------------------------------------------
//  Subscribes to the requested name and returns the number of subscribers
//  to the element.

int
TnmsWxTree::Subscribe ( const std::string & absoluteName, TreeSubscriber * sub )
{
    SubscriberMap::iterator  sIter;

    bool r  = _visible->subscribe(absoluteName, sub);
    int  sz = 0;
    sIter   = _subs.find(absoluteName);

    if ( sIter == _subs.end() && r )
    {
        TnmsTree::NotifySet  subset;
        subset.insert(sub);
        _subs[absoluteName] = subset;
        _stree->tree->subscribe(absoluteName, _stree->notifier);
        sz++;
    }
    else if ( sIter != _subs.end() )
    {
        if ( r ) {
            TnmsTree::NotifySet & subset = sIter->second;
            subset.insert(sub);
            sz = subset.size();
        } else {
            _subs.erase(sIter);
        }
    }

    return sz;
}

// ----------------------------------------------------------------------
//  UnSubscribes to the requested name and returns the number of subscribers
//  to the element.

int
TnmsWxTree::Unsubscribe ( const std::string & absoluteName, TreeSubscriber * sub )
{
    SubscriberMap::iterator  sIter;

    _visible->unsubscribe(absoluteName, sub);

    int sz = 0;
    sIter  = _subs.find(absoluteName);
    
    if ( sIter != _subs.end() )
    {
        TnmsTree::NotifySet & subset = sIter->second;
        subset.erase(sub);
        sz = subset.size();
        if ( subset.empty() ) {
            _subs.erase(sIter);
            _stree->tree->unsubscribe(absoluteName, _stree->notifier);
        }
    }

    return sz;
}

// ----------------------------------------------------------------------

wxTreeItemId 
TnmsWxTree::RecursiveAdd ( TnmsTree::Node * node )
{
    wxString      name;
    wxTreeItemId  id;
    TreeItem    * data = NULL;

    if ( NULL == node->getParent() )
        id = _rootId;
    else
    {
        TreeIdMap::iterator  tIter;
        tIter = _idMap.find(node->getParent()->getAbsoluteName());

        if ( tIter == _idMap.end() )
            id = this->RecursiveAdd(node->getParent());
        else
            id = tIter->second;
    }

    name  = wxString::FromAscii(node->getName().c_str());
    data  = new TreeItem(node->getAbsoluteName());
    id    = _treeCtrl->AppendItem(id, name, -1, -1, data);

    _idMap[node->getAbsoluteName()] = id;

    if ( id.IsOk())
        _treeCtrl->SetItemHasChildren(id);

    return id;
}

// ----------------------------------------------------------------------


void
TnmsWxTree::Add ( TnmsTree::Node * node )
{
    if ( node == NULL )
        return;

    _visible->add(node->getAbsoluteName());
    this->RecursiveAdd(node);

    return;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void
TnmsWxTree::Sync()
{
    ClientSubscriber * notifier = _stree->notifier;

    if ( ! notifier->haveUpdates() && ! _visible->empty() )
        return;

    if ( _visible->empty() )
        _stree->tree->subscribe("/", _stree->notifier);

    //LogFacility::LogMessage("TnmsWxTree::Sync() ");

    TreeIdMap::iterator      pIter;
    TreeUpdateSet::iterator  nIter;
    TreeRemoveSet::iterator  rIter;

    wxString         name;
    wxTreeItemId     pid, id;

    if ( ! notifier->trylock() )
        return;

    // Adds
    TreeUpdateSet & adds = notifier->adds;
    for ( nIter = adds.begin(); nIter != adds.end(); )
    {
        TnmsTree::Node * node = *nIter;

        if ( node->getValue().erase ) {
            adds.erase(nIter++);
            continue;
        }

        if ( _visible->exists(node->getAbsoluteName()) ) {
            _visible->update(node->getValue().metric);
            LogFacility::LogMessage("TnmsWxTree::Sync() Add " + node->getAbsoluteName());
        } else {
            this->Add(node);
        }

        adds.erase(nIter++);
    }

    // Removes
    TreeRemoveSet & removes = notifier->removes;
    for ( rIter = removes.begin(); rIter != removes.end(); ) 
    {
        if ( _visible->exists(*rIter) ) 
        {
            pIter = _idMap.find(*rIter);
            pid   = pIter->second;
            _treeCtrl->Delete(pid);  // CRASHOLA

            //erase now..should change to sweep() on interval so we see our deletes better
            _visible->remove(*rIter);
            _idMap.erase(pIter);
        }

        removes.erase(rIter++);
    }

    TreeUpdateSet & updates = notifier->updates;
    for ( nIter = updates.begin(); nIter != updates.end(); ) 
    {
        TnmsTree::Node * node = *nIter;

        LogFacility::LogMessage("TnmsWxTree::Sync() Update " + node->getAbsoluteName());

        if ( ! _visible->exists(node->getAbsoluteName()) )
            this->Add(node);
        else
            _visible->update(node->getValue().metric);

        updates.erase(nIter++);
    }

    //_stree->tree->debugDump();
    _stree->notifier->unlock();

    _visible->updateSubscribers();

    return;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void
TnmsWxTree::SetTnmsTree ( TnmsTree_R * tree )
{
    _stree = tree;

    if ( tree == NULL )
        return;

    if ( _stree->mutex->lock() )
    {
        _stree->tree->subStructure((TreeSubscriber*) _stree->notifier);
        _stree->mutex->unlock();
    }

    return;
}

// ----------------------------------------------------------------------

TnmsTree_R*
TnmsWxTree::GetTnmsTree()
{
    return _stree;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// _TNMSWXTREE_CPP_

