#define _METRICLISTVIEW_CPP_

#include "MetricListView.h"

#include "LogFacility.h"
using namespace tcanetpp;



const char* vt_names[] = 
{ 
    "TNMS_NONE", 
    "TNMS_INT32", 
    "TNMS_UINT32", 
    "TNMS_INT64", 
    "TNMS_UINT64", 
    "TNMS_FLOAT", 
    "TNMS_STRING"
};


MetricListView::MetricListView ( wxWindow         * parent, 
                                 const wxWindowID   id,
                                 const wxPoint    & pos,
                                 const wxSize     & size,
                                 long               style )
    : wxListCtrl(parent, id, pos, size, style|wxLC_REPORT|wxLC_HRULES),
      _sub(new TnmsSubscriber()),
      _lastid(0),
      _default(-1),
      _syncEnabled(true)
{
    this->Init();
}

MetricListView::~MetricListView()
{
}


void
MetricListView::Init()
{
    wxListItem  itemCol;

    itemCol.SetText(_T("Name"));
    itemCol.SetImage(-1);
    itemCol.SetWidth(35);
    this->InsertColumn(0, itemCol);

    itemCol.SetText(_T("Value Type"));
    itemCol.SetWidth(10);
    this->InsertColumn(1, itemCol);

    itemCol.SetText(_T("Value"));
    itemCol.SetWidth(25);
    this->InsertColumn(2, itemCol);

    itemCol.SetText(_T("Data"));
    itemCol.SetWidth(35);
    this->InsertColumn(3, itemCol);

    this->AddDefaultItem();
}

void
MetricListView::AddDefaultItem()
{
    wxListItem  mitem;
    long        id;

    id = this->InsertItem(this->getNewId(), _T("/"), 0);
    _default = id;

    this->SetItemData(id, id);
    this->SetItem(id, 1, _T("NOVALUE"));
    this->SetItem(id, 2, _T(" "));
    this->SetItem(id, 3, _T("Default Entry"));

    mitem.m_itemId = id;
    mitem.SetTextColour(*wxBLUE);
    mitem.SetFont(*wxITALIC_FONT);
    mitem.SetBackgroundColour(*wxLIGHT_GREY);
    this->SetItem(mitem);
    
    this->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
    this->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
    this->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
    this->SetColumnWidth(3, wxLIST_AUTOSIZE);

    return;
}


long
MetricListView::getNewId()
{
    long id = _lastid;
    _lastid++;
    return id;
}

TreeSubscriber*
MetricListView::Subscriber()
{
    return _sub;
}


bool
MetricListView::AddMetric ( TnmsMetric & metric )
{
    wxString    name;
    wxListItem  mitem;
    long        id;

    if ( _default > -1 ) {
        this->DeleteItem(_default);
        _default = -1;
    }

    name = wxString::FromAscii(metric.getElementName().c_str());
    id   = this->InsertItem(this->getNewId(), name, 0);

    _metrics[metric.getElementName()] = id;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("MetricListView::AddMetric()"); 

    return this->UpdateMetric(id, metric);
}


bool
MetricListView::UpdateMetric ( long id, TnmsMetric & metric )
{
    wxString    name, valT, val;
    std::string vt;

    name = wxString::FromAscii(metric.getElementName().c_str());
    vt   = vt_names[metric.getValueType()];
    valT = wxString::FromAscii(vt.c_str());

    if ( metric.getValueType() == TNMS_STRING )
        val = wxString::FromAscii(metric.getValue().c_str());
    else
        val = wxString::FromAscii(StringUtils::toString(metric.getValue<uint64_t>()).c_str());

    this->SetItem(id, 1, valT);
    this->SetItem(id, 2, val);
    this->SetItem(id, 3, _T(" "));

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("MetricListView::UpdateMetric() " 
            + metric.getElementName());

    return true;
}


bool
MetricListView::RemoveMetric ( const std::string & name )
{
    MetricListMap::iterator mIter;

    mIter = _metrics.find(name);

    if ( mIter == _metrics.end() )
        return false;

    this->DeleteItem(mIter->second);
    _metrics.erase(mIter);

    LogFacility::LogMessage("MetricListView::RemoveMetric() " + name);

    return true;
}

bool
MetricListView::Unsubscribe ( const std::string & name )
{
    MetricListMap::iterator  mIter;
    bool result = false;

    for ( mIter = _metrics.begin(); mIter != _metrics.end(); ++mIter )
    {
        if ( StringUtils::startsWith(mIter->first, name) )
        {
            this->DeleteItem(mIter->second);
            _metrics.erase(mIter);
            result = true;
        }
    }

    return result;
}


void
MetricListView::Sync()
{
    if ( ! _sub->haveUpdates() || ! _syncEnabled ) {
        return;
    }

    this->Hide();

    //LogFacility::LogMessage("MetricListView::Sync()");

    TreeUpdateSet::iterator  nIter;
    TreeRemoveSet::iterator  rIter;
    MetricListMap::iterator  mIter;

    TreeUpdateSet & adds = _sub->adds;
    for ( nIter = adds.begin(); nIter != adds.end(); )
    {
        TnmsTree::Node * node = *nIter;

        mIter = _metrics.find(node->getAbsoluteName());

        if ( mIter != _metrics.end() ) {
            this->UpdateMetric(mIter->second, node->getValue().metric);
        } else {
            this->AddMetric(node->getValue().metric);
        }

        adds.erase(nIter++);
    }

    TreeRemoveSet & removes = _sub->removes;
    for ( rIter = removes.begin(); rIter != removes.end(); ) 
    {
        if ( ! this->RemoveMetric(*rIter) )
            LogFacility::LogMessage("MetricListView::Sync() error removing metric " 
                + *rIter);
/*
        mIter = _metrics.find(*rIter);
        if ( mIter != _metrics.end() ) {
            this->DeleteItem(mIter->second);
            _metrics.erase(mIter);
        }
*/
        removes.erase(rIter++);
    }

    TreeUpdateSet & updates = _sub->updates;
    for ( nIter = updates.begin(); nIter != updates.end(); ) 
    {
        TnmsTree::Node * node = *nIter;

        mIter = _metrics.find(node->getAbsoluteName());

        if ( mIter != _metrics.end() ) {
            this->UpdateMetric(mIter->second, node->getValue().metric);
        } else {
            this->AddMetric(node->getValue().metric);
        }
        updates.erase(nIter++);
    }

    this->Show();

    return;
}


void
MetricListView::EnableSync()
{
    _syncEnabled = true;
}


void
MetricListView::DisableSync()
{
    _syncEnabled = false;
}


// _METRICLISTVIEW_CPP_

