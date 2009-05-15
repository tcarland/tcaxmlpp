#ifndef _TNMSCORE_TNMSAGENT_H_
#define _TNMSCORE_TNMSAGENT_H_

#include "tnmsCore.h"
#include "TnmsClient.h"

namespace tnmsCore {


class TnmsAgent : public TnmsClient {

  public:

    TnmsAgent();

    virtual ~TnmsAgent();

    //virtual void    AuthReplyHandler    ( const TnmsAuthReply   & reply );
    virtual void    AuthRequestHandler  ( const TnmsAuthRequest & request ) {}

    void        flushLimit  ( uint32_t flush );
    uint32_t    flushLimit();

    void        login();

    void        clear();
    int         flush();

    std::string getConfig() { return _xmlconfig; }


  private:

    uint32_t     _flushlimit;
    std::string  _xmlconfig;


};


} // namespace

#endif  // _TNMSCORE_TNMSAGENT_H_

