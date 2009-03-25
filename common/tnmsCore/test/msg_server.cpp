#define _server_cc

extern "C" {
# include <unistd.h>
# include <sys/select.h>
# include <errno.h>
# include <signal.h>
# include <stdio.h>
}

#include <cstdlib>
#include <vector>
#include <string>


#include "LogFacility.h"
#include "CidrUtils.h"
#include "TnmsSocket.h"
#include "TestMessageHandler.hpp"
using namespace tcanetpp;
using namespace tnmsCore;
using namespace tnmsTest;


BufferedSocket*  createServer   ( int port );
//int              recvClientData ( TnmsSocket *client );
void             sigHandler     ( int signal );


// globals
bool    _Alarm = false;





BufferedSocket*
createServer ( int port )
{
    int     retry  = 0;

    BufferedSocket *server = NULL;

    while ( retry < 4 ) {

	try {
	    server = new BufferedSocket(0, port, SOCKET_SERVER, IPPROTO_TCP);
	    if ( ! server->init(true) ) {
		printf("Socket error: %s\n", server->errorStr().c_str());
	    } else {
		server->setNonBlocking();
		server->setSocketOption(SocketOption::SetRcvBuf(65535));
                printf("Server listening: %s\n", server->getAddrStr().c_str());
		return server;
	    }
	} catch ( SocketException err ) {
	    printf("Socket exception: %s: Retrying...\n", err.toString().c_str());
	    delete server;
	}
	retry++;
	sleep(15);
    }

    printf("Fatal error creating server socket\n");
    return NULL;
}


int
recvClientData ( TnmsSocket * client )
{
    int          rd;
    time_t       now;
    
    now = time(NULL);

    do {
	rd = client->receive(now);

	if ( rd < 0 ) {
            printf("recvClient error for %s : %s\n", 
                client->getHostStr().c_str(), client->getErrorStr().c_str());
	    return -1;
	} else if ( rd == 0 ) {
            printf("no data\n");
	    return 0;
	}

	printf("bytes read = %d\n", rd);

    } while ( rd > 0 );

    return 1;
}


void
sigHandler ( int signal )
{
    if ( signal == SIGINT || signal == SIGTERM )
	_Alarm = true;
    return;
}




int main ( int argc, char **argv )
{
    BufferedSocket      *server, *client;

    fd_set   rset;
    int      rdy;

    std::vector<TnmsSocket*>            clist;
    std::vector<TnmsSocket*>::iterator  cIter;

    if ( argc == 1 ) {
	printf("Usage: server [port]\n");
	exit(0);
    }
    
    
    int port   = atoi(argv[1]);
    int maxfdp = 128;

    signal(SIGINT, &sigHandler);
    signal(SIGTERM, &sigHandler);
    signal(SIGPIPE, SIG_IGN);

    FD_ZERO(&rset);

    if ( (server = createServer(port)) == NULL ) {
	printf("Fatal error creating server socket\n");
	exit(-1);
    }
    
    LogFacility::OpenLogFile("msgsvr", "msgsvr.log", false);

    while ( ! _Alarm ) {
	struct timeval to;

	FD_SET(server->getFD(), &rset);

	for ( cIter = clist.begin(); cIter != clist.end(); cIter++ )
	    FD_SET((*cIter)->getFD(), &rset);
	
	bzero(&to, sizeof(to));
	to.tv_sec = 5;

	if ( (rdy = select(maxfdp, &rset, NULL, NULL, &to)) < 0 ) {
	    if ( errno == EINTR )
		continue;
	    else
		printf("select error\n");
	}

	if ( FD_ISSET(server->getFD(), &rset) ) {
	    if ( (client = (BufferedSocket*) server->accept(BufferedSocket::factory)) != NULL ) {
                TnmsSocket * svrclient = new TnmsSocket(client, new TestMessageHandler());
		clist.push_back(svrclient);
		printf("Client accepted from %s\n",
		    svrclient->getHostStr().c_str());
                continue;
	    }
	}

        printf("clist size is %d\n", clist.size());

	for ( u_int i = 0; i < clist.size(); i++ ) {
	    TnmsSocket *c = (TnmsSocket*) clist[i];
            if ( c == NULL ) {
                printf("no client object\n");
                continue;
            }

	    if ( FD_ISSET(c->getFD(), &rset) ) {

		if ( recvClientData(c) < 0 ) {
		    printf("Read Error: Closing client %s\n", c->getHostStr().c_str());
		    FD_CLR(c->getFD(), &rset);
		    delete c;
		    clist.erase(clist.begin() + i);
		}

	    }
	}
    }

    printf("Alarm received, cleaning up\n");

    LogFacility::CloseLogFacility();

    for ( cIter = clist.begin(); cIter != clist.end(); cIter++ )
	delete *cIter;
    clist.clear();
    delete server;

    return 0;
}



