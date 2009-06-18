#define _TNMSCORE_TNMSAGENT_CPP_

#include "TnmsAgent.h"


namespace tnmsCore {


TnmsAgent::TnmsAgent( TnmsTree * tree )
    : TnmsClient(tree) 
{
    this->_agent  = true;
    this->_mirror = false;
}

void
TnmsAgent::login ( const std::string & agentname ) 
{
    std::string  login = TNMS_AGENT_ID;

    login.append(":").append(agentname);
    TnmsClient::login(login, "");
    
    return;
}


}  // namespace

// _TNMSCORE_TNMSAGENT_CPP_

