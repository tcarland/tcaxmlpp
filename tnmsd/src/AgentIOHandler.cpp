#define _TNMSD_AGENTIOHANDLER_CPP_

#include "AgentIOHandler.h"


namespace tnmsd {


AgentIOHandler::AgentIOHandler()
    : _tree(NULL)
{}

AgentIOHandler::AgentIOHandler ( TnmsTree * tree )
    : _tree(tree)
{}

AgentIOHandler::~AgentIOHandler() {}


void
AgentIOHandler::handle_accept ( const EventIO * io )
{

}

void
AgentIOHandler::handle_read ( const EventIO * io )
{

}

void
AgentIOHandler::handle_write ( const EventIO * io )
{

}

void
AgentIOHandler::handle_close ( const EventIO * io )
{

}

void
AgentIOHandler::handle_shut ( const EventIO * io )
{

}

void
AgentIOHandler::handle_destroy ( const EventIO * io )
{
    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client )
        delete client;

    return;
}


bool
AgentIOHandler::readable ( const EventIO * io )
{
    return true;
}


bool
AgentIOHandler::writeable ( const EventIO * io )
{
    if ( io == NULL || io->isServer )
        return false;

    TnmsClient * client = (TnmsClient*) io->rock;

    if ( client && client->getTxQueueSize() > 0 )
        return true;

    return false;
}


} // namespace 


/*  _TNMSD_AGENTIOHANDLER_CPP_  */

