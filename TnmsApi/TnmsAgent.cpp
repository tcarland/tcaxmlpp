#define _TNMSCORE_TNMSAGENT_CPP_

#include "TnmsAgent.h"


namespace tnmsApi {


TnmsAgent::TnmsAgent( TnmsTree * tree )
    : TnmsClient(tree) 
{
    this->_isAgent  = true;
    this->_isMirror = false;
}

TnmsAgent::~TnmsAgent() {}

void
TnmsAgent::login ( const std::string & user ) 
{
    std::string  login = TNMS_AGENT_ID;

    login.append(":").append(user);
    this->setClientLogin(login, "");
    TnmsClient::login();
    
    return;
}


}  // namespace

// _TNMSCORE_TNMSAGENT_CPP_

