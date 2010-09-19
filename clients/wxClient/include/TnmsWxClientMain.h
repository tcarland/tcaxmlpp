#ifndef _TNMSWXCLIENT_MAIN_H_
#define _TNMSWXCLIENT_MAIN_H_


#include <wx/wx.h>
#include <wx/timer.h>


#include "TnmsTree_R.hpp"

#define TGUITIMER_ID  6999
#define TINTERVAL_MS  3000


class ClientFrame;


class TnmsWxClientMain : public wxApp {

  public:

    TnmsWxClientMain ( int msecs = TINTERVAL_MS );

    virtual ~TnmsWxClientMain();


    virtual bool   OnInit();
    virtual int    OnRun();
    virtual int    OnExit();
    
    virtual int    MainLoop();

    virtual void   OnTimer ( wxTimerEvent & event );


  private:

    TnmsTree_R          _stree;
    wxTimer             _guiTimer;

    ClientFrame *       _mframe;

    static std::string  _Version;

    DECLARE_EVENT_TABLE()
};


#endif  // _TNMSWXCLIENT_MAIN_H_

