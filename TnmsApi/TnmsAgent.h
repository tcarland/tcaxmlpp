#ifndef _TNMSCORE_TNMSAGENT_H_
#define _TNMSCORE_TNMSAGENT_H_

#include "tnmsCore.h"


namespace tnmsCore {

#define TNMS_AGENT_ID "tnmsagent" 


class TnmsAgent : public TnmsSocket {

  public:

    TnmsAgent();

    virtual ~TnmsAgent();


    void        flushLimit  ( uint32_t flush );
    uint32_t    flushLimit();

    void        login();
    void        clear();
    int         flush();


    // queueAdd
    // queueUpdate
    // queueRremove
    // authReply()

    std::string getConfig() { return _xmlconfig; }


  private:

    uint32_t     _flushlimit;
    std::string  _xmlconfig;


};


} // namespace

#endif  // _TNMSCORE_TNMSAGENT_H_

