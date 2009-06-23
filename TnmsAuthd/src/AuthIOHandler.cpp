#define _TNMSAUTH_AUTHIOHANDLER_CPP_


#include "AuthIOHandler.h"
#include "TnmsAuthThread.h"
#include "TnmsAuthClient.h"


namespace tnmsauth { 


AuthIOHandler::AuthIOHandler ( TnmsAuthThread * authThread )
    : _authThread(authThread)
{}

void
AuthIOHandler::timeout ( const time_t & now )
{

}

void
AuthIOHandler::handle_accept ( const EventIO * io )
{
    Socket         * svr    = (Socket*) io->rock;
    TnmsAuthClient * client = NULL;
    BufferedSocket * sock   = NULL;

    sock = (BufferedSocket*) svr->accept(BufferedSocket::factory);

    if ( NULL == sock ) 
        return;

    client = new TnmsAuthClient(sock, _authThread);
    io->evmgr->addIOEvent(this, client->getFD(), client);
    _clients.insert(client);

    return;
}

void
AuthIOHandler::handle_read ( const EventIO * io )
{
}

void
AuthIOHandler::handle_write ( const EventIO * io )
{
}

void
AuthIOHandler::handle_destroy ( const EventIO * io )
{
}

bool
AuthIOHandler::readable ( const EventIO * io )
{
}


bool
AuthIOHandler::writeable ( const EventIO * io )
{
}

}  // namespace

// _TNMSAUTH_AUTHIOHANDLER_CPP_

