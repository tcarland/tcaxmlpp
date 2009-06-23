#define _TNMSAUTH_SOAPCLIENT_CPP_

#include "SoapClient.h"

#include "CidrUtils.h"
#include "StringUtils.h"
#include "LogFacility.h"
using namespace tcanetpp;

#include "soapStub.h"
#include "tnms.nsmap"


namespace tnmsauth {


SoapClient::SoapClientFactory SoapClient::factory;


SoapClient::SoapClient ( const int & fd, struct soap * tsoap )
    : _soap(tsoap),
      _fd(fd),
      _svr(false),
      _ssl(false)
{
    _ipaddr = CidrUtils::ntop(ntohl(tsoap->ip));
}

SoapClient::SoapClient ( const std::string & pemfile )
    : _soap(NULL),
      _fd(-1),
      _pem(pemfile),
      _ipaddr(0),
      _svr(false),
      _ssl(false)
{
    this->_soap = soap_new();
}


SoapClient::~SoapClient()
{
    if ( _soap ) {
        if ( ! _svr ) {
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
    if ( LogFacility::Debug() )
        LogFacility::LogMessage("SoapClient::handle_event(): " + _ipaddr);

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

    _svr    = true;
    _ipaddr = "0.0.0.0";

#ifdef WITH_OPENSSL
    if ( _pem.length() > 0 ) {
        if ( ! FileUtils::isReadable(_pem.c_str()) ) {
            _errstr = "SoapClient SSL PEM file not readable";
            return -1;
        } else {
            if ( soap_ssl_server_context(_soap, SOAP_SSL_DEFAULT, _pem.c_str(), 
                                         NULL, NULL, NULL, NULL, NULL, NULL) ) 
            {
                _errstr = "SoapClient::bind() SSL context init failed";
                return -1;
            }
        }
    }
#endif

    _soap->user = userobj;
    _fd         = soap_bind(_soap, _ipaddr.c_str(), port, 20);

    if ( _fd < 0 ) {
        _errstr = "SoapClient::bind() failed";
        return _fd;
    }

    return _fd;
}


const int&
SoapClient::getFD() const
{
    return _fd;
}


SoapClient*
SoapClient::accept ( SoapClientFactory & factory )
{
    struct soap * tsoap = NULL;
    int           fd    = -1;

    fd = soap_accept(_soap);

    if ( fd < 0 ) 
        return NULL;

    tsoap       = soap_copy(_soap);
    tsoap->user = _soap->user;

#ifdef WITH_OPENSSL
    if ( _pem.length() > 0 ) {
        if (soap_ssl_accept(tsoap) != SOAP_OK) {
            _errstr = "  Error in soap_ssl_accept() "; 
            _errstr.append(StringUtils::toString(tsoap->error));
            soap_destroy(tsoap);
            soap_end(tsoap);
            soap_done(tsoap);
            free(tsoap);
            return NULL;
        }

        _ssl = true;;
    }
#endif

    return( (SoapClient*) factory(fd, tsoap) );
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
    return _errstr;
}

bool
SoapClient::sslEnabled() const
{
    return _ssl;
}


} // namespace

//  _TNMSAUTH_SOAPCLIENT_CPP_

