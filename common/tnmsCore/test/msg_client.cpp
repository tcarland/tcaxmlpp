#define _client_cc

extern "C" {
# include <signal.h>
# include <stdio.h>
# include <unistd.h>
# include <time.h>
}


#include "CidrUtils.h"
#include "TnmsSocket.h"
#include "TestMessageHandler.hpp"
using namespace tcanetpp;
using namespace tnmsCore;
using namespace tnmsTest;


bool _Alarm = false;


void
sigHandler ( int signal )
{
    if ( signal == SIGINT || signal == SIGTERM )
        _Alarm = true;
    return;
}



int main ( int argc, char **argv )
{
    int          wt, rd, port;
    ipv4addr_t   addr; 

    std::string  host = "localhost";

    TnmsSocket        * sock = NULL;

    CidrUtils::pton("127.0.0.1", addr);

    if ( argc == 1 ) {
	printf("Usage: client [server_port]\n");
	exit(0);
    }

    printf("Connecting to %s\n", CidrUtils::ntop(addr).c_str());

    port = atoi(argv[1]);

    //sock = new BufferedSocket(addr, port, SOCKET_CLIENT, IPPROTO_TCP);
    //sock->setDebug(true);
    sock = new TnmsSocket(new TestMessageHandler());

    if ( sock->openConnection(host, port) < 0 ) {
        printf("Error connecting to server: %s\n", sock->getErrorStr().c_str());
        return -1;
    }

    //sock->setNonBlocking();

    signal(SIGINT, &sigHandler);
    signal(SIGTERM, &sigHandler);
    signal(SIGPIPE, SIG_IGN);

    //bzero(&foo, sizeof(foo_t));
    //foo.client = addr;

    TnmsAdd add("foo");
    sock->subscribeStructure();
    sock->sendMessage(&add);
    sock->send();
    
    while ( ! _Alarm ) {
	
	//foo.timestamp = (uint32_t) time(NULL);
	//foo.count     += 1;
        //

        if ( (wt = sock->send()) < 0 ) {
	    printf("msgclient: Write error\n");
	    sock->close();
	    sock->openConnection();
        } else if ( (rd = sock->receive()) < 0 ) {
	    printf("msgclient: Read error\n");
	    sock->close();
	    sock->openConnection();
	} else {
	    printf("msgclient: Wrote %d bytes\n", wt);
	}
	sleep(5);

    }
    printf("Terminating..\n");

    if ( sock->isConnected() )
	sock->close();

    delete sock;

    return 0;
}

