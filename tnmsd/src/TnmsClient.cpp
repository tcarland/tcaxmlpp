#define _TNMSD_TNMSCLIENT_CPP_


#include "TnmsClient.h"

namespace tnmsd {

TnmsClient::TnmsClient ( TnmsTree * tree )
    : _tree(tree),
      _msgHandler(new TnmsMessageHandler(tree, this))
{
}

TnmsClient::~TnmsClient()
{
    delete _msgHandler;
}


bool
TnmsClient::isMirrorClient()
{
    return _isMirror;
}


void
TnmsClient::queueAdd()
{

}

void
TnmsClient::queueRemove()
{

}


void
TnmsClient::queueUpdate()
{

}

void
TnmsClient::close()
{

}




} // namespace


// _TNMSD_TNMSCLIENT_CPP_

