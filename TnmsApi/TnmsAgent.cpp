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
    std::string  login = user;

    login.append(":").append(user);
    TnmsClient::login(user, "tnmsagent");
    
    return;
}


}  // namespace

// _TNMSCORE_TNMSAGENT_CPP_

