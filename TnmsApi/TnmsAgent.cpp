#define _TNMSCORE_TNMSAGENT_CPP_

#include "TnmsAgent.h"

namespace tnmsCore {



TnmsAgent::TnmsAgent( TnmsTree * tree )
    : TnmsClient(tree) 
{
    this->_agent = true;
}


TnmsAgent::~TnmsAgent() 
{}


void
TnmsAgent::flushLimit ( uint32_t flush ) 
{
    _flushLimit = flush;
}


uint32_t
TnmsAgent::flushLimit()
{
    return _flushLimit; 
}


void
TnmsAgent::login ( const std::string & agentname ) 
{
    std::string  login = TNMS_AGENT_ID;

    login.append(":").append(agentname);
    TnmsClient::login(login, "");
    
    return;
}


void
TnmsAgent::clear ( const std::string & metric )
{
    this->queueRemove(metric);
}


int
TnmsAgent::flush()
{
    return 0;
}



}  // namespace

// _TNMSCORE_TNMSAGENT_CPP_

