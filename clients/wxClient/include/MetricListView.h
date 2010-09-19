#ifndef _METRICLISTVIEW_H_
#define _METRICLISTVIEW_H_

#include <wx/listctrl.h>

#include "ClientSubscriber.h"


#define TNMS_ID_MLIST  7005


typedef std::map<std::string, long>  MetricListMap;



class MetricListView : public wxListCtrl {

  public:

    MetricListView ( wxWindow * parent, const wxWindowID   id,
                                        const wxPoint    & pos,
                                        const wxSize     & size,
                                        long               style);

    virtual ~MetricListView();


    bool        AddMetric      ( TnmsMetric & metric );
    bool        UpdateMetric   ( long id, TnmsMetric & metric );
    bool        RemoveMetric   ( const std::string & name );


    void        Sync();


    TreeSubscriber*  Subscriber();

    void        EnableSync();
    void        DisableSync();

  private:

    void        Init();
    void        AddDefaultItem();
    long        getNewId();

  private:

    TnmsSubscriber * _sub;

    MetricListMap    _metrics;

    long             _lastid;
    long             _default;

    bool             _syncEnabled;
};


#endif  // _METRICLISTVIEW_H_

