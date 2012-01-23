#define _TNMSCLIENT_TNMSWXTREECTL_CPP_

#include <iostream> 


#include "TnmsWxTreeCtl.h"
#include "ClientSubscriber.h"
#include "ClientTreeMutex.h"

#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsClient {

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------



// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

TnmsWxTreeCtl::TnmsWxTreeCtl()
    : _treeCtrl(NULL)
{
    this->Init();
}

TnmsWxTreeCtl::TnmsWxTreeCtl ( wxWindow        * parent,
                               const wxWindowID  id,
                               const wxString  & name,
                               const wxPoint   & pos,
                               const wxSize    & size,
                               long              style )
{
    this->Init();
    this->Create(parent, id, name, pos, size, style);
}


TnmsWxTreeCtl::~TnmsWxTreeCtl()
{
    if ( _treeCtrl )
        delete _treeCtrl;
    delete _visible;
}

// ----------------------------------------------------------------------

void
TnmsWxTreeCtl::Init()
{
    _rootId  = 0;
    _visible = new TnmsTree();
}


bool
TnmsWxTreeCtl::Create  ( wxWindow        * parent,
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

    Connect(TNMS_ID_TREE, wxEVT_SIZE, wxSizeEventHandler(TnmsWxTreeCtl::OnSize));

    return true;
}

// ----------------------------------------------------------------------

TreeItem*
TnmsWxTreeCtl::GetItemData ( wxTreeItemId  id )
{
    return( (TreeItem*) _treeCtrl->GetItemData(id) );
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void
TnmsWxTreeCtl::OnSize ( wxSizeEvent & WXUNUSED(event) )
{
    this->DoResize();
}

// ----------------------------------------------------------------------

void
TnmsWxTreeCtl::OnSelect ( wxTreeEvent & event )
{
    //wxTreeItemId   id   = event.GetItem();
    //TreeItem     * data = (TreeItem*) _treeCtrl->GetItemData(id);
    //LogFacility::LogMessage("OnSelect " + data->absoluteName);
    return;
}

// ----------------------------------------------------------------------

void
TnmsWxTreeCtl::OnDelete ( wxTreeEvent & event )
{
    //LogFacility::LogMessage("TnmsWxTreeCtl::OnDelete ");
}

// ----------------------------------------------------------------------

void
TnmsWxTreeCtl::DoResize()
{
    wxSize sz = GetClientSize();
    _treeCtrl->SetSize(0, 0, sz.x, sz.y);
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

TnmsMetric
TnmsWxTreeCtl::GetItemMetric ( wxTreeItemId & id )
{
    TnmsMetric   metric;
    TreeItem   * data = this->GetItemData(id);
    
    _visible->request(data->absoluteName, metric);

    return metric;
}

// ----------------------------------------------------------------------
/**  Subscribes to the requested name and returns the number of subscribers
  *  to the element.
 **/
int
TnmsWxTreeCtl::Subscribe ( const std::string & absoluteName, TreeSubscriber * sub )
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
        TnmsTree * t = _mtree->acquireTree();
        if ( t != NULL ) {
            t->subscribe(absoluteName, _mtree->subscriber());
            sz++;
            _mtree->releaseTree();
        }
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
/**  Unsubscribes to the requested name and returns the number of subscribers
  *  to the element.
 **/
int
TnmsWxTreeCtl::Unsubscribe ( const std::string & absoluteName, TreeSubscriber * sub )
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
            TnmsTree * t = _mtree->acquireTree();
            if ( t != NULL ) {
                t->unsubscribe(absoluteName, _mtree->subscriber());
                _mtree->releaseTree();
            }
        }
    }

    return sz;
}

// ----------------------------------------------------------------------

wxTreeItemId 
TnmsWxTreeCtl::RecursiveAdd ( TnmsTree::Node * node )
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

        if ( id.IsOk()) {
            _treeCtrl->SetItemHasChildren(id);
            _treeCtrl->SortChildren(id);
        }
    }

    name  = wxString::FromAscii(node->getName().c_str());
    data  = new TreeItem(node->getAbsoluteName());
    id    = _treeCtrl->AppendItem(id, name, -1, -1, data);

    _idMap[node->getAbsoluteName()] = id;

    return id;
}

// ----------------------------------------------------------------------

void
TnmsWxTreeCtl::Add ( TnmsTree::Node * node )
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
TnmsWxTreeCtl::Sync()
{
    ClientSubscriber * notifier = _mtree->subscriber();

    if ( ! notifier->haveUpdates() && ! _visible->empty() )
        return;

    if ( _visible->empty() ) {
        TnmsTree * t = _mtree->acquireTree();
        if ( t ) {
            t->subscribe("/", _mtree->subscriber());
            _mtree->releaseTree();
        }
    }

    //LogFacility::LogMessage("TnmsWxTreeCtl::Sync() ");

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
            LogFacility::LogMessage("TnmsWxTreeCtl::Sync() Add " + node->getAbsoluteName());
        } else {
            this->Add(node);
        }

        adds.erase(nIter++);
    }

    // Removes
    TreeRemoveSet & removes = notifier->removes;
    for ( rIter = removes.begin(); rIter != removes.end(); ) 
    {
        LogFacility::LogMessage("TnmsWxTreeCtl::Sync() Remove " + *rIter);
        if ( _visible->exists(*rIter) ) 
        {
            pIter = _idMap.find(*rIter);
            pid   = pIter->second;
            _treeCtrl->Delete(pid);  // CRASHOLA

            //erase now..should change to sweep() on interval so we see our deletes better
            _visible->remove(*rIter);
            _idMap.erase(pIter);
        } else
            _idMap.erase(*rIter);

        removes.erase(rIter++);
    }

    TreeUpdateSet & updates = notifier->updates;
    for ( nIter = updates.begin(); nIter != updates.end(); ) 
    {
        TnmsTree::Node * node = *nIter;

        LogFacility::LogMessage("TnmsWxTreeCtl::Sync() Update " + node->getAbsoluteName());

        if ( ! _visible->exists(node->getAbsoluteName()) )
            this->Add(node);
        else
            _visible->update(node->getValue().metric);

        updates.erase(nIter++);
    }

    notifier->unlock();

    _visible->updateSubscribers();

    return;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

void
TnmsWxTreeCtl::SetTnmsTree ( ClientTreeMutex * tree )
{
    _mtree = tree;

    if ( tree == NULL )
        return;

    TnmsTree * t = _mtree->acquireTree();

    if ( t != NULL ) {
        t->subStructure((TreeSubscriber*) _mtree->subscriber());
        _mtree->releaseTree();
    }

    return;
}

// ----------------------------------------------------------------------

ClientTreeMutex*
TnmsWxTreeCtl::GetTnmsTree()
{
    return _mtree;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

} // namespace

// _TNMSWXTREE_CPP_

