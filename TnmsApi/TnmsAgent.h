#ifndef _TNMSCORE_TNMSAGENT_H_
#define _TNMSCORE_TNMSAGENT_H_

#include "tnmsCore.h"
#include "TnmsClient.h"

namespace tnmsCore {


class TnmsAgent : public TnmsClient {

  public:

    TnmsAgent();

    virtual ~TnmsAgent();

    virtual void    AuthRequestHandler  ( const TnmsAuthRequest & request ) {}


    void        login ( const std::string & agentname);


};


} // namespace

#endif  // _TNMSCORE_TNMSAGENT_H_

