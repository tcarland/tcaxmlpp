#define _TNMSCORE_AUTHIOHANDLER_CPP_

namespace tnmsCore {


void
AuthIOHandler::handle_read ( const EventIO * io )
{
    TnmsClient * client = (TnmsClient*) io->rock;

    if  ( client && client->receive() < 0 )
        client->close();

    return;
}

void
AuthIOHandler::handle_write ( const EventIO * io )
{
    TnmsClient * client = (TnmsClient*) io->rock;

    if  ( client && client->receive() < 0 )
        client->close();

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

void
AuthIOHandler::readable ( const EventIO * io )
{
    return true;
}

void
AuthIOHandler::writeable ( const EventIO * io )
{
    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client && client->txBytesBuffered() > 0 )
        return true;

    return false;
}

}  // namespace

// _TNMSCORE_AUTHIOHANDLER_CPP_ 

