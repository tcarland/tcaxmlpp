#define _client_cc

extern "C" {
# include <signal.h>
# include <stdio.h>
# include <unistd.h>
# include <time.h>
}

#include "CidrUtils.h"
#include "BufferedSocket.h"

#include "foo.h"

using namespace tcanetpp;

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
    foo_t       foo;
    int         wt, port;
    ipv4addr_t  addr; 

    BufferedSocket  * sock = NULL;

    CidrUtils::pton("127.0.0.1", addr);

    if ( argc == 1 ) {
	printf("Usage: client [server_port]\n");
	exit(0);
    }

    printf("Connecting to %s\n", CidrUtils::ntop(addr).c_str());

    port = atoi(argv[1]);

    sock = new BufferedSocket(addr, port, SOCKET_CLIENT, IPPROTO_TCP);
    //sock->setDebug(true);

    if ( sock->connect() < 0 ) {
        printf("Error connecting to server: %s\n", sock->errorStr().c_str());
        return -1;
    }

    sock->setNonBlocking();

    signal(SIGINT, &sigHandler);
    signal(SIGTERM, &sigHandler);
    signal(SIGPIPE, SIG_IGN);

    bzero(&foo, sizeof(foo_t));
    foo.client = addr;

    while ( ! _Alarm ) {
	
	foo.timestamp = (uint32_t) time(NULL);
	foo.count     += 1;

	if ( (wt = sock->write((void*)&foo, sizeof(foo_t))) < 0 ) {
	    printf("Write error\n");
	    sock->close();
	    sock->init(false);
	    sock->connect();
	} else {
	    printf("Wrote %d bytes\n", wt);
	}
	sleep(5);

    }
    printf("Terminating..\n");

    if ( sock->isConnected() )
	sock->close();

    delete sock;

    return 0;
}

