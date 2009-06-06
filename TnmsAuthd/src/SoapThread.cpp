#define _TNMSAUTH_SOAPTHREAD_CPP_


#include "SoapThread.h"
#include "SoapClient.h"


namespace tnmsauth {


SoapThread::SoapThread ( SoapClient * client, const time_t & now )
    : _client(client),
      _started(now)
{}

SoapThread::~SoapThread()
{
    if ( _client )
        delete _client;
}


void
SoapThread::run()
{
    if ( _client )
        _client->handle_event();
    return;
}


time_t
SoapThread::started()
{
    return this->_started;
}


}  // namespace

// _TNMSAUTH_SOAPTHREAD_CPP_

