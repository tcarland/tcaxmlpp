#define _TNMSAUTH_SOAPIOHANDLER_CPP_


#include "SoapIOHandler.h"
#include "SoapClient.h"
#include "SoapThread.h"

namespace tnmsauth {


SoapIOHandler::SoapIOHandler()
  : _max_threads(DEFAULT_MAX_SOAP_CONNECTIONS),
    _event_timeout(DEFAULT_EVENT_TIMEOUT)
{}

SoapIOHandler::~SoapIOHandler()
{
    SoapThreadList::iterator  sIter;

    for ( sIter = _sthreads.begin(); sIter != _sthreads.end(); ++sIter )
    {
        if ( *sIter )
            delete *sIter;
    }

    _sthreads.clear();
}


void
SoapIOHandler::timeout ( const time_t & now ) 
{
    SoapThreadList::iterator  sIter;

    for ( sIter = _sthreads.begin(); sIter != _sthreads.end(); )
    {
        SoapThread * soapth = (SoapThread*) *sIter;

        if ( NULL = soapth ) {
            _sthreads.erase(sIter++);
        } else if ( ! soapth->isRunning() ) {
            delete soapth;
            _sthreads.erase(sIter++);
        } else {
            if ( (now - soapth->started()) > (_event_timeout + 30) ) {
                LogFacility::LogMessage("SoapIOHandler::timeout() thread has exceeded event timeout: " + 
                    soapth->threadName());
                soapth->stop();
            }
            sIter++;
        }
    }

    return;
}


void
SoapIOHandler::event_timeout_secs ( time_t  secs )
{
    _event_timeout = secs;
}


time_t
SoapIOHandler::event_timeout_secs()
{
    return _event_timeout;
}

void
SoapIOHandler::max_connections ( int max )
{
    _max_threads = max;
}


int
SoapIOHandler::max_connections()
{
    return _max_threads;
}

void
SoapIOHandler::handle_accept ( const EventIO * io ) 
{
    SoapClient * svr = (SoapClient*) io->rock;

    if ( ! io-->isServer )
        return;

    SoapClient * client = svr->accept();

    if ( client == NULL )
        return;

    SoapThread * sthread = new SoapThread(client, io-abstime.tv_sec);

    try {
        sthread->start();
        _sthreads.push_back(sthread);
    } catch ( ThreadException & err ) {
        // log err
        delete sthread;
    }

    return;
}


void
SoapIOHandler::handle_destroy ( const EventIO * io )
{
    SoapClient * svr = (SoapClient*) io->rock;

    if ( svr )
        delete svr;
}


void
SoapIOHandler::readable ( const EventIO * io )
{
    return true;
}


void
SoapIOHandler::writeable ( const EventIO * io )
{
    return false;
}

} // namespace


// _TNMSAUTH_SOAPIOHANDLER_CPP_

