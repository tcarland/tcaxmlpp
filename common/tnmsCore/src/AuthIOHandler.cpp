#define _TNMSCORE_AUTHIOHANDLER_CPP_


#include "AuthIOHandler.h"
#include "TnmsClient.h"

#include "EventManager.h"
#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsCore {


void
AuthIOHandler::handle_read ( const EventIO * io )
{
    TnmsClient * client = (TnmsClient*) io->rock;

    if  ( client && client->receive(io->abstime.tv_sec) < 0 ) {
        LogFacility::LogMessage("AuthIOHandler::handle_read() error: " + client->getHostStr());
        client->close();
    }

    return;
}

void
AuthIOHandler::handle_write ( const EventIO * io )
{
    TnmsClient * client = (TnmsClient*) io->rock;

    if  ( client && client->send(io->abstime.tv_sec) < 0 ) {
        LogFacility::LogMessage("AuthIOHandler::handle_write() error: " + client->getHostStr());
        client->close();
    }

    return;
}

void
AuthIOHandler::handle_close ( const EventIO * io )
{
    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client ) {
        LogFacility::LogMessage("AuthIOHandler::handle_close() " + client->getHostStr());
        client->close();
    }

    return;
}

bool
AuthIOHandler::readable ( const EventIO * io )
{
    return true;
}

bool
AuthIOHandler::writeable ( const EventIO * io )
{
    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client && client->txBytesBuffered() > 0 )
        return true;

    return false;
}

}  // namespace

// _TNMSCORE_AUTHIOHANDLER_CPP_ 

