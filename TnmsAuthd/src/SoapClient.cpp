#define _TNMSAUTH_SOAPCLIENT_CPP_

#include "SoapClient.h"

#include "soapStub.h"
#include "TnmsAuth.nsmap"

#include "IpAddr.h"
#include "StringUtils.h"
#include "FileUtils.h"
#include "LogFacility.h"
using namespace tcanetpp;



namespace tnmsauth {


SoapClient::SoapClientFactory SoapClient::factory;


SoapClient::SoapClient ( const int & fd, struct soap * tsoap )
    : _soap(tsoap),
      _fd(fd),
      _isSvr(false)
{
    _ipaddr = IpAddr::ntop(ntohl(tsoap->ip));
}

SoapClient::SoapClient ( const std::string & pemfile )
    : _soap(soap_new()),
      _fd(-1),
      _pem(pemfile),
      _isSvr(false)
{
}


SoapClient::~SoapClient()
{
    if ( _soap ) {
        if ( ! _isSvr ) {
            soap_destroy(_soap);
            soap_end(_soap);
        }
        soap_done(_soap);
        ::free(_soap);
    }
}


bool
SoapClient::handle_event()
{
    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("SoapClient::handle_event(): " + _ipaddr);

    //if ( _soap->serve() != SOAP_OK ) {
    if ( soap_serve(_soap) != SOAP_OK ) {
        LogFacility::LogMessage("SoapClient::handle_event() soap error: " 
            + StringUtils::toString(_soap->error));
        return false;
    }
    this->close();

    return true;
}


void
SoapClient::close()
{}


int
SoapClient::bind ( uint16_t port, void * userobj )
{
    _soap->bind_flags      = SO_REUSEADDR;
    _soap->send_timeout    = 45;
    _soap->recv_timeout    = 45;
    _soap->max_keep_alive  = 100;

    _isSvr    = true;
    _ipaddr = "0.0.0.0";

#ifdef WITH_OPENSSL
    if ( _pem.length() > 0 ) {
        if ( ! FileUtils::IsReadable(_pem) ) {
            _errStr = "SoapClient SSL PEM file not readable";
            return -1;
        } else {
            soap_ssl_init(); 
            if ( soap_ssl_server_context(_soap, SOAP_SSL_DEFAULT, _pem.c_str(), 
                                         NULL, NULL, NULL, NULL, NULL, NULL) ) 
            {
                _errStr = "SoapClient::bind() SSL context init failed";
                return -1;
            }
        }
    }
#endif

    _soap->user = userobj;
    _fd         = soap_bind(_soap, _ipaddr.c_str(), port, 20);
    //_fd        = _soap->bind(_ipaddr.c_str(), port, 20);

    if ( _fd < 0 ) {
        _errStr = "SoapClient::bind() failed";
        return _fd;
    }

    return _fd;
}


SoapClient*
SoapClient::accept ( SoapClientFactory & factory )
{
    struct soap * tsoap = NULL;
    //TnmsAuthService * tsoap = NULL;
    int  fd  = -1;

    fd = soap_accept(_soap);
    //fd = _soap->accept();

    if ( fd < 0 ) 
        return NULL;

    tsoap       = soap_copy(_soap);
    tsoap->user = _soap->user;
    //tsoap       = _soap->copy();
    //tsoap->user = _soap->user;

#ifdef WITH_OPENSSL
    if ( _pem.length() > 0 ) {
        if ( soap_ssl_accept(tsoap) != SOAP_OK ) {
            _errStr = "  Error in soap_ssl_accept() "; 
            _errStr.append(StringUtils::toString(tsoap->error));
            soap_destroy(tsoap);
            soap_end(tsoap);
            soap_done(tsoap);
            free(tsoap);
            return NULL;
        }

    }
#endif

    return( (SoapClient*) factory(fd, tsoap) );
}

SoapClient*
SoapClient::accept()
{
    return this->accept(factory);
}

const int&
SoapClient::getFD() const
{
    return _fd;
}

const std::string&
SoapClient::getAddrStr() const
{
    return _ipaddr;
}

const std::string&
SoapClient::getErrorStr() const
{
    return _errStr;
}

bool
SoapClient::sslEnabled() const
{
    return(! _pem.empty());
}


} // namespace

//  _TNMSAUTH_SOAPCLIENT_CPP_

