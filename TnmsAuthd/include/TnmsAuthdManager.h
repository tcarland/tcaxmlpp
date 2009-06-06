#ifndef TNMSAUTH_TNMSAUTHDMANAGER_H_
#define TNMSAUTH_TNMSAUTHDMANAGER_H_


#include "EventManager.h"


namespace tnmsauth {


class TnmsAuthdManager : public EventTimerHandler {

  public:

    TnmsAuthdManager();
    virtual ~TnmsAuthdManager();


    // EventTimerHandler
    virtual void timeout ( const EventTimer * timer );


    void         run();


    void         setAlarm();
    void         setDebug ( bool debug = true );

    std::string  getErrorStr();

  protected:

    bool         parseConfig ( const std::string & cfg,
                               const time_t      & now );

    void         logRotate   ( std::string     logfile,
                               const time_t  & now );

  private:

    EventManager*       _evmgr;
    TnmsTree*           _tree;

    Socket*             _svr;

    evid_t              _svrid;


    SoapIOHandler*      _soapHandler;
    AuthIOHandler*      _authHandler;

    time_t              _lastTouched;
    time_t              _logRotate;
    time_t              _reportAt;
    time_t              _holddown;
    int                 _today;

    std::string         _configfile;
    std::string         _errstr;

    bool                _hup;
    bool                _debug;

};

  
}  // namespace

#endif  //  TNMSAUTH_TNMSAUTHDMANAGER_H_

