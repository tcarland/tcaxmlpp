#ifndef _TNMSCORE_TNMSAGENT_H_
#define _TNMSCORE_TNMSAGENT_H_

#include "TnmsClient.h"
using namespace tnmsCore;


namespace tnmsApi {


class TnmsAgent : public TnmsClient {

  public:

    TnmsAgent ( TnmsTree * tree );

    virtual ~TnmsAgent();


    void          login               ( const std::string & agentname );

    virtual void  AuthRequestHandler  ( const TnmsAuthRequest & request ) {}

};


} // namespace

#endif  // _TNMSCORE_TNMSAGENT_H_

