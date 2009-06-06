#ifndef _INCLUDE_SOAPCLIENT_H_
#define _INCLUDE_SOAPCLIENT_H_

#include <string>


namespace tnmsauth {


#define uSec *-1
#define mSec *-1000

#define SOAP_RECV_TIMEOUT 30
#define SOAP_SEND_TIMEOUT 30


class SoapClient {

  public:

    class SoapClientFactory {
      public:
        explicit SoapClientFactory() {}
        virtual ~SoapClientFactory() {}

        virtual SoapClient* operator() ( const int & fd, struct soap * tsoap )
        {
            return( new SoapClient(fd, tsoap) );
        }
    };

  protected:
    
    SoapClient ( const int & fd, struct soap * tsoap );

  public:

    SoapClient ( const std::string & pemfile = "" );
    virtual ~SoapClient();

    virtual bool  handle_event();
    virtual void  close()    {}

    const int&    getFD() const;
    std::string   getAddrStr() const;

    int           bind       ( uint16_t port, void * userobj );
    SoapClient*   accept     ( SoapClientFactory & factory );
    SoapClient*   accept();


  protected:

    static SoapClientFactory  factory;

  protected:

    struct soap*        _soap;
    int                 _fd;

    std::string         _pem;
    std::string         _ipaddr;

    bool                _svr;

};


} // namespace

#endif  // _INCLUDE_SOAPCLIENT_H_

