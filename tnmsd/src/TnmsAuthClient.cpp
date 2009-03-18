#define _TNMSD_TNMSAUTHCLIENT_CPP_

#include "TnmsAuthClient.h"


namespace tnmsd {


TnmsAuthClient::TnmsAuthClient ( EventManager * evmgr )
    : _evmgr(evmgr)
{}

TnmsAuthClient::~TnmsAuthClient()
{}

bool
TnmsAuthClient::authenticate ( TnmsClient * client, tnmsAuthRequest & authreq )
{}

void
TnmsAuthClient::removeClient ( TnmsClient * client )
{}

void
TnmsAuthClient::connectAuth()
{}

void
TnmsAuthClient::close()
{}

void
TnmsAuthClient::AuthReplyHandler ( tnmsAuthReply & reply )
{}

void
TnmsAuthClient::timeout ( const EventTimer * timer )
{
}

void
TnmsAuthClient::handle_read ( const EventIO * io )
{
}

void
TnmsAuthClient::handle_close ( const EventIO * io )
{}

void
TnmsAuthClient::handle_write ( const EventIO * io )
{}

void
TnmsAuthClient::handle_destroy ( const EventIO * io )
{}

void
TnmsAuthClient::readable ( const EventIO * io )
{}

void
TnmsAuthClient::writable ( const EventIO * io )
{}


}  // namespace

// _TNMSD_TNMSAUTHCLIENT_CPP_

