#ifndef _TCANETPP_WHOIS_H_
#define _TCANETPP_WHOIS_H_

#include <string>

#include "Socket.h"


namespace tcanetpp  {


#define MAX_READS_IN_BLOCK 20
#define DEFAULT_WHOIS_PORT 43


class Whois {

  public:

    Whois ( const std::string & host = "" );
    virtual ~Whois();


    std::string   query ( const std::string & query, 
                          const std::string & host = "" );

    std::string   getErrorStr() const;

  protected:

    void          init  ( const std::string & host, uint16_t port = 0 );

  private:

    Socket *            _sock;
    std::string         _host;
    std::string         _errstr;

};

}  // namespace

#endif  // _TCANETPP_WHOIS_H_

