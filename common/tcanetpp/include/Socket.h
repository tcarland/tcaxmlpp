#ifndef SOCKET_H_
#define SOCKET_H_

#ifdef WIN32
# include <Winsock2.h>
# pragma warning (disable:4267)
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
#endif

#include <string>
#include <cstdlib>

#include "tcanet_types.h"
#include "Exception.hpp"
#include "SocketOption.h"


namespace tcanetpp {


#ifdef WIN32
typedef SOCKET sockfd_t;
#else
typedef int    sockfd_t;
#endif


/**
  *   Socket types for Socket constructor
 **/
enum SocketType
{
	SOCKET_NONE,
	SOCKET_CLIENT,
	SOCKET_SERVER,
	SOCKET_SERVER_CLIENT,
	SOCKET_RAW
};



class SocketException : public Exception {
public:
	SocketException ( const std::string & errstr ) 	    : Exception(errstr) {}
	virtual ~SocketException() throw() {}
};



class Socket {
	
public:

	class SocketFactory {
	public:
		virtual ~SocketFactory() {}
		
		virtual Socket* operator() ( sockfd_t & fd, struct sockaddr_in & csock )
		{
			return new Socket(fd, csock);
		}
	};

	static SocketFactory  factory;
	
public:
	
	Socket();
	Socket ( ipv4addr_t ipaddr, uint16_t port, SocketType type, int protocol )
		throw ( SocketException );
	
	virtual ~Socket();
	
	
protected:
	
	Socket ( sockfd_t & fd, struct sockaddr_in & csock );
	
	
public:
	
	virtual int         init ( bool block = false );
	
	int                 bind();
	int                 listen();
	
	virtual int         connect();
	virtual void        close();
	virtual void        shutdown  ( int shut );
	
	Socket*             accept();
	Socket*             accept ( SocketFactory & factory );
	
	bool                isConnected();
	bool                isServerSocket();
	
	ipv4addr_t          getAddress();
	std::string         getAddressString();
	std::string         getAddrString()  { return this->getAddressString(); }
	
	const sockfd_t&     getDescriptor() const;
	const sockfd_t&     getFD() const { return this->getDescriptor(); }
	
	const SocketType&   getSocketType() const;
	const int&          getSocketProtocol() const;
	const uint16_t&     getPort() const;

	int                 setSocketOption ( int optname, int optval );
	int                 setSocketOption ( SocketOption sockopt );
	
	void                setBlocking();
	void                setNonBlocking();
	bool                isBlocking();
	
	const std::string&  getErrorString() const;
	const std::string&  errorStr() const { return this->getErrorString(); }
	
	
	virtual ssize_t     read  ( void * vptr, size_t n );
	virtual ssize_t     write ( const void * vptr, size_t n );
		
	
  public:	
	
	static void         Block   ( Socket * s );
	static void         Unblock ( Socket * s );
	
	static bool         IsValidDescriptor ( const sockfd_t & fd );
	static void         ResetDescriptor ( sockfd_t & fd );

	
protected:
	
	ssize_t             nwriten  ( const void * vptr, size_t n );
	ssize_t             nreadn   ( void * vptr, size_t n );
	
	
	static void InitializeSocket ( sockfd_t & fd, int & proto )
		throw ( SocketException );
	
	
	static ipv4addr_t   pton     ( const std::string & ipstr );
	static std::string  ntop     ( ipv4addr_t addr );
	
	
protected:
	
	std::string				_addrstr;
	std::string				_errstr;
	
private:
	
	sockfd_t                _fd;
	struct sockaddr_in      _sock;
	SocketType              _socktype;
	int                     _proto;
	uint16_t                _port;
	
	bool                    _bound;
	bool                    _connected;
	bool                    _block;
	
};


} // namespace

#endif /*SOCKET_H_*/
