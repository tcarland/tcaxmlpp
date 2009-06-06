#define _TNMSAUTH_SOAPTHREAD_CPP_


#include "SoapThread.h"


namespace tnmsauth {


SoapThread::SoapThread ( SoapClient * client, const time_t & now )
    : _client(client),
      _started(now)
{}

SoapThread::~SoapThread() {}


void
SoapThread::run()
{
    return;
}


time_t
SoapThread::started()
{
    return this->_started;
}

}  // namespace


// _TNMSAUTH_SOAPTHREAD_CPP_

