#define _METRICLISTVIEW_CPP_

#include "MetricListView.h"




const char* vt_names[] = 
{ "TNMS_NONE", "TNMS_INT32", 
  "TNMS_UINT32", "TNMS_INT64", 
  "TNMS_UINT64", "TNMS_FLOAT", 
  "TNMS_STRING"
};


MetricListView::MetricListView ( wxWindow * parent, const wxWindowID   id,
                                                    const wxPoint    & pos,
                                                    const wxSize     & size,
                                                    long               style )
    : wxListCtrl(parent, id, pos, size, style|wxLC_REPORT),
      _lastid(1),
      _default(0)
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
    this->InsertColumn(0, itemCol);

    itemCol.SetText(_T("Value Type"));
    this->InsertColumn(1, itemCol);

    itemCol.SetText(_T("Value"));
    this->InsertColumn(2, itemCol);

    itemCol.SetText(_T("Data"));
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

bool
MetricListView::AddMetric ( TnmsMetric & metric )
{
    wxListItem  mitem;
    long        id;

    if ( _lastid == 0 )
        return false;   // ie. we're full, should throw manage id's

    wxString name = wxString::FromAscii(metric.getElementName().c_str());

    id = this->InsertItem(this->getNewId(), name, 0);

    wxString  valT, val;

    std::string  vt = vt_names[metric.getValueType()];
    valT = wxString::FromAscii(vt.c_str());
    if ( metric.getValueType() == TNMS_STRING )
        val = wxString::FromAscii(metric.getValue().c_str());
    else
        val = wxString::FromAscii(StringUtils::toString(metric.getValue<uint64_t>()).c_str());
   
    this->SetItem(id, 1, valT);
    this->SetItem(id, 2, val);
    this->SetItem(id, 3, _T(" "));

    return true;
}


// _METRICLISTVIEW_CPP_

