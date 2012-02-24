#define _client_cc

extern "C" {
# include <signal.h>
# include <stdio.h>
# include <unistd.h>
# include <time.h>
}

#include "LogFacility.h"
#include "IpAddr.h"
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
    TnmsSocket * sock = NULL;

    IpAddr::pton("127.0.0.1", addr);

    if ( argc == 1 ) {
	printf("Usage: client [server_port]\n");
	exit(0);
    }

    port = atoi(argv[1]);
    sock = new TnmsSocket((new TestMessageHandler()));

    printf("Connecting to %s:%d\n", host.c_str(), port);

    int c = sock->openConnection(host, port);

    if ( c < 0 ) {
        printf("Error connecting to server: %s\n", sock->getErrorStr().c_str());
        return -1;
    } else if ( c == 0 ) {
        printf("..connecting..");
        c = sock->connect();
    }

    signal(SIGINT, &sigHandler);
    signal(SIGTERM, &sigHandler);
    signal(SIGPIPE, SIG_IGN);

    LogFacility::OpenLogFile("msgclient", "msgclient", "msgclient.log", false);

    TnmsMetric metric1("foo/bar");
    TnmsAdd add("foo/bar");

    int i = 1;
    time_t now = ::time(NULL);

    metric1.setValue(TNMS_INT32, i);

    sock->subscribeStructure();
    sock->sendMessage(&add);
    sock->send(now);
    
    while ( ! _Alarm ) {

        now = ::time(NULL);

        if ( ! sock->isConnected() ) {
            printf("Not connected...");
            c = sock->connect();
            if ( c < 0 ) {
                printf("Error connecting to server: %s\n", sock->getErrorStr().c_str());
                sock->close();
            } else if ( c == 0 ) {
                printf("..connecting..\n");
                sock->connect();
            } 
        } else {
            if ( c == 1 ) {
                printf("Connected.\n");
                c = 0;
            }
            sock->sendMessage(&metric1);

            if ( (wt = sock->send(now)) < 0 ) {
                printf("msgclient: Write error %s\n", sock->getErrorStr().c_str());
                sock->close();
            } else if ( (rd = sock->receive(now)) < 0 ) {
                printf("msgclient: Read error %s\n", sock->getErrorStr().c_str());
                sock->close();
            } else {
                printf("msgclient: Wrote %d bytes\n", wt);
            }
            i++;
        }
        sleep(5);
    }
    if ( sock->isConnected() )
	sock->close();

    printf("Terminating..\n");

    delete sock;

    return 0;
}

