#define _TNMSCLIENTIOTHREAD_CPP_


#include "TnmsClientIOThread.h"




TnmsClientIOThread::TnmsClientIOThread
TnmsClientIOThread::~TnmsClientIOThread()
{}


void
TnmsClientIOThread::run()
{

}

void
TnmsClientIOThread::addClient ( TnmsClient * client )
{
    _clients.insert(client);
}

void
TnmsClientIOThread::removeClient ( TnmsClient * client )
{
    _clients.erase(client);
}


