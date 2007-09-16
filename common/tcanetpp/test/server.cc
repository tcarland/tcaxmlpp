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


#include "foo.h"

#include "CidrUtils.h"
#include "BufferedSocket.h"


using namespace tcanetpp;



BufferedSocket*  createServer   ( int port );
int              recvClientData ( BufferedSocket *client, char* buffer );
void             sigHandler     ( int signal );


// globals
bool    _Alarm = false;


std::vector<BufferedSocket*>            clist;
std::vector<BufferedSocket*>::iterator  cIter;


int main ( int argc, char **argv )
{
    BufferedSocket  *server, *client;
    fd_set   rset;
    char    *buffer;
    int      rdy;

    if ( argc == 1 ) {
	printf("Usage: server [port]\n");
	exit(0);
    }
    
    int port = atoi(argv[1]);

    signal(SIGINT, &sigHandler);
    signal(SIGTERM, &sigHandler);
    signal(SIGPIPE, SIG_IGN);

    FD_ZERO(&rset);

    int maxfdp = 128;

    if ( (server = createServer(port)) == NULL ) {
	printf("Fatal error creating server socket\n");
	exit(-1);
    }

    buffer = (char*) malloc(sizeof(foo_t));

    if ( buffer == NULL ) {
	printf("Fatal: buffer malloc failed\n");
	exit(-1);
    }


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
		clist.push_back(client);
		printf("Client connection accepted from %s\n",
		    client->getAddrString().c_str());
	    }
	}

	for ( u_int i = 0; i < clist.size(); i++ ) {
	    BufferedSocket *c = (BufferedSocket*) clist[i];
	    if ( FD_ISSET(c->getFD(), &rset) ) {
		if ( recvClientData(c, buffer) < 0 ) {
		    printf("Closing client %s\n", c->getAddrString().c_str());
		    FD_CLR(c->getFD(), &rset);
		    delete c;
		    clist.erase(clist.begin() + i);
		}
	    }
	}
    }

    printf("Alarm received, cleaning up\n");

    for ( cIter = clist.begin(); cIter != clist.end(); cIter++ )
	delete *cIter;
    clist.clear();
    delete server;

    return 0;
}


int
recvClientData ( BufferedSocket * client, char * buffer )
{
    foo_t       *head;
    int          rd;
    uint32_t     stamp;

    do {
	rd = client->read(buffer, sizeof(foo_t));

	if ( rd != sizeof(foo_t) && rd > 0 ) {
	    printf("Read error rcvd %d != %d\n", rd, sizeof(foo_t));
	    return 0;
	} else if ( rd < 0 ) {
	    return -1;
	} else if ( rd == 0 ) {
	    return 0;
	}
	printf("bytes read = %d\n", rd);

	head = (foo_t*) buffer;

	stamp = head->timestamp;

	printf("Processed foo_t from %s timestamp: %u count: %u\n",
	    CidrUtils::ntop(head->client).c_str(), stamp, head->count);

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


