#define _tnmsauth_test_cpp_

#include "Thread.h"
#include "StringUtils.h"

#include "soapH.h"
#include "TnmsAuth.nsmap"

#define TESTUSER  "tcanms"
#define TESTPASS  "tcanms11b"


//#define DEFAULT_URL      "https://localhost:15990"
#define DEFAULT_URL      "http://localhost:15902"
#define DEFAULT_TIMEOUT  30



class TestAuthThread : public tcanetpp::Thread {

  public:

    TestAuthThread ( const std::string & url )
        : Thread(true),
          _soap(soap_new()),
          _url(url),
          _result(5)
    {}

    virtual ~TestAuthThread()
    {
        soap_done(_soap);
        free(_soap);
        _soap = NULL;
    }

    void run()
    {
        std::string  user, pw, tk, ip;
        int          ret = 0;

        struct ns1__AuthResponse  r;
        r.result.ticket  = 0;
        r.result.message = 0;
        r.result.success = false;
        r.result.timeout = 0;

        user = TESTUSER;
        pw   = TESTPASS;
        ip   = "0.0.0.0";

        const char * action = "";

        char  * userc = strdup(user.c_str());
        char  * passc = strdup(pw.c_str());
        char  * addrc = strdup(ip.c_str());
        char  * urlc  = strdup(_url.c_str());

        _result = 1;

        ret = soap_call_ns1__authenticate(_soap, urlc, action,
            userc, passc, addrc, r);

        if ( ret != SOAP_OK )
        {
            _result = 2;
            _errStr = "Fatal error in soap call: ";
            _errStr.append(tcanetpp::StringUtils::toString(_soap->error));
            return;
        }

        if  ( r.result.success )
            _result = 6;
        else
            _result = 3;

        return;
    }

    int  auth_result()
    {
        return _result;
    }

    std::string  errorStr()
    {
        return this->_errStr;
    }

  private:
    
    struct soap *   _soap;
    std::string     _url;
    std::string     _errStr;
    int             _result;
};

//------------------------------------------------------------

static 
const char TNMSAUTHTEST_VERSION [] = "0.10";
static
const char process [] = "tnmsauth_test";



void 
version()
{
    printf("%s Version: %s\n", process, TNMSAUTHTEST_VERSION);
    return;
}


void
usage()
{
    printf("\nUsage: %s <timeout> [url]\n", process);
    printf("    default url  =  '%s'\n\n", DEFAULT_URL);
    printf(" Result codes:\n");
    printf("      6  -  OK\n");
    printf("      5  -  Invalid Test - test did not run\n");
    printf("      4  -  Invalid Test - thread error\n");
    printf("      3  -  Failure - no data or auth failure\n");
    printf("      2  -  Failure - soap error\n");

}



/*
 *    Return codes:
 *
 *    6  -  OK
 *    5  -  Invalid Test - test did not run
 *    4  -  Invalid Test - thread error
 *    3  -  Failure - no data or auth failure
 *    2  -  Failure - soap error
 *    1  -  Failure - timeout for response
 *   
 */


int main ( int argc, char **argv )
{
    std::string  url;
    int          val;
    time_t       timeout, start, now;

    val   = 5;
    now   = 0;
    start = ::time(NULL);

    if ( argc < 2 ) {
        version();
        usage();
        return val;
    }
 
    if ( argc >= 2 )
        timeout = atoi(argv[1]);
    else
        timeout = DEFAULT_TIMEOUT;

    if ( argc == 3 )
        url = argv[2];
    else
        url = DEFAULT_URL;
   
    TestAuthThread * auth_th = new TestAuthThread(url);

    try {
        auth_th->start();
    } catch ( tcanetpp::ThreadException & err ) {
        val = 4;
        delete auth_th;
        return val;
    }

    printf("Waiting for result\n");
 
    do {
        sleep(1);
        now  = time(NULL);
        val  = auth_th->auth_result();

        if ( val == 6 ) {
            printf("Success! Authentication succeeded to %s\n", url.c_str());
            break;
        }
    } while ( (auth_th->isRunning()) && ((now - start) < timeout) );

    printf("\n");

    auth_th->stop();
    val = auth_th->auth_result();

    if ( val != 6 )
        printf("Auth request failed: %d\n", val);

    delete auth_th;

    return val;
}

