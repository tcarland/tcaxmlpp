#define _TCANETPP_WHOIS_CPP_

#include <stdlib.h>
#include <math.h>


#include "Whois.h"
#include "CidrUtils.h"


namespace tcanetpp {


Whois::Whois ( const std::string & host )
    : _sock(NULL)
{
    this->init(host);
}


Whois::~Whois()
{
    if ( _sock ) {
        _sock->close();
        delete _sock;
    }
}


void
Whois::init ( const std::string & host, uint16_t port )
{
    if ( port == 0 )
        port = DEFAULT_WHOIS_PORT;

    if ( _sock != NULL )
    {
        if ( host == _host && port == DEFAULT_WHOIS_PORT )
            return;
        _sock->close();
        delete _sock;
        _sock = NULL;
    }

    if ( host.empty() )
        return;

    IpAddrList   addrs;
    CidrUtils::GetHostAddrList(host, addrs);
    if ( addrs.empty() ) { // error
        _errstr = "GetHostAddrList failed to resolve any addresses";
        return;
    }

    IpAddrList::iterator  hIter;
    for ( hIter = addrs.begin(); hIter != addrs.end(); ++hIter )
    {
        IpAddr & addr = *hIter;

        if ( ! addr.ipv4() )
            continue;

        if ( _sock )
            delete _sock;

        _sock = new Socket(addr.getAddr4(), port, SOCKTYPE_CLIENT, SOCKET_TCP);
        _sock->setBlocking();
        if ( _sock->connect() > 0 )
            break;
        _sock->close();
    }

    if ( ! _sock->isConnected() ) 
    {
        delete _sock;
        _sock = NULL;
    }
    _host  = host;

    return;
}


std::string
Whois::query ( const std::string & query, const std::string & host )
{
    std::string  str, result;
    ssize_t      rr  = 0;
    char         ch;
    ldiv_t       ld;
    long         x, y = 10;

    if ( ! host.empty() )
        this->init(host);

    if ( _sock == NULL ) { // throw or error
        _errstr = "Failed to init socket";
        return result;
    }

    str = query;
    str.append("\r\n");

    if ( ! _sock->isBlocking() )
        _sock->setBlocking();

    rr = _sock->write(str.data(), str.length());

    if ( rr < 0 )
        return result;

    _sock->setNonBlocking();
    sleep(1);

    x = 0;
    while ( (rr = _sock->read(&ch, sizeof(ch))) >= 0 )
    {
        if ( rr == 0 ) {
            x++;
            ld = ldiv(x, y);
            if ( x >= MAX_READS_IN_BLOCK ) {
                break;
            } else if ( ld.rem == 0 && result.empty() ) {
                sleep(1);
            }
            continue;
        }
        x = 0;
        result += ch;
    }

    return result;
}

std::string
Whois::getErrorStr() const
{
    return _errstr;
}

}  // namespace



#ifdef TWHOIS_MAIN

#include <iostream>

using namespace tcanetpp;


void usage()
{
    std::cout << "Usage: whois -h host querystring" << std::endl;
}

int main ( int argc, char ** argv )
{
    std::string  host, query;
    char *       hoststr = NULL;
    char         optChar;
    bool         verbose = false;

    while ( (optChar = getopt(argc, argv, "h:v")) != EOF )
    {
        switch ( optChar ) {
            case 'h':
                hoststr = strdup(optarg);
                break;
            case 'v':
                verbose = true;
                break;
            default:
                usage();
                break;
        }
    }

    if ( argc > 3 ) {
        int ac = 3;
        query.append(argv[ac]);
        ac++;
        while ( ac < argc ) {
            query.append(" ").append(argv[ac]);
            ac++;
        }
    }

    if ( hoststr != NULL && strlen(hoststr) > 0 )
    {
        host = hoststr;
        free(hoststr);
    }
    else
        usage();

    std::cout << " host  = " << host << std::endl;
    std::cout << " query = " << query << std::endl << std::endl;
     
    Whois  who(host);
    std::string r = who.query(query);
    std::cout << " result = '" << r << "'" << std::endl;

    return 0;
}

#endif  // TWHOIS_MAIN

// _TCANETPP_WHOIS_CPP_
