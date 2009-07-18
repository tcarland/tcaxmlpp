#define _TNMSCLIENTIOTHREAD_CPP_


#include "TnmsClientIOThread.h"




TnmsClientIOThread::TnmsClientIOThread ( TnmsTree_R * tree )
    : 
TnmsClientIOThread::~TnmsClientIOThread()
{}


void
TnmsClientIOThread::run()
{

}

void
TnmsClientIOThread::addClient ( TnmsClient * client )
{
    if ( client == NULL )
        return;

    _clients.insert(client);
}

void
TnmsClientIOThread::removeClient ( TnmsClient * client )
{
    _clients.erase(client);
}


