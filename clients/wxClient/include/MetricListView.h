#ifndef _METRICLISTVIEW_H_
#define _METRICLISTVIEW_H_

#include <wx/listctrl.h>

#include "ClientSubscriber.h"

namespace tnmsclient {


#define TNMS_ID_MLIST  7005

typedef std::map<std::string, long>  MetricListMap;



class MetricListView : public wxListCtrl {

  public:

    MetricListView ( wxWindow         * parent,
                     const wxWindowID   id,
                     const wxPoint    & pos,
                     const wxSize     & size,
                     long               style);

    virtual ~MetricListView();


    bool        AddMetric      ( TnmsMetric & metric );
    bool        UpdateMetric   ( long id, TnmsMetric & metric );
    bool        RemoveMetric   ( const std::string & name );
    bool        Unsubscribe    ( const std::string & name );

    void        Sync();
    void        EnableSync();
    void        DisableSync();


    TreeSubscriber*  Subscriber();


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

} // namespace

#endif  // _METRICLISTVIEW_H_

