#define _TNMSCORE_TNMSAGENT_CPP_

#include "TnmsAgent.h"

namespace tnmsCore {



TnmsAgent::TnmsAgent() {}

TnmsAgent::~TnmsAgent() {}

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
TnmsAgent::login() {}

void
TnmsAgent::clear() {}

int
TnmsAgent::flush()
{
    return 0;
}




}  // namespace

// _TNMSCORE_TNMSAGENT_CPP_

