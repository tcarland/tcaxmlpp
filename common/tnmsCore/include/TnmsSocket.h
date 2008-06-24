#ifndef _TNMSSOCKET_H_
#define _TNMSSOCKET_H_

#include "BufferedSocket.h"
using namespace tcanetpp;



namespace tnmscore {


class TnmsSocket : public tcanetpp::BufferedSocket {

public:

    class TnmsSocketFactory : public tcanetpp::Socket::SocketFactory {
    public:
        virtual ~TnmsSocketFactory() {}

        virtual Socket* operator() ( sockfd_t & fd, struct sockaddr_in & csock )
        {
            return ( (Socket*) new TnmsSocket(fd, csock) );
        }
    };

    static TnmsSocketFactory  factory;


protected:

    TnmsSocket ( sockfd_t & fd, stuct sockaddr_in & csock );

public:

    TnmsSocket();
    TnmsSocket ( ipv4addr_t ip, uint16_t port, SocketType socktype, int proto );

    virtual ~TnmsSocket();


};


} // namespace

#endif  // _TNMSSOCKET_H_

