#ifndef _TNMSAUTH_AUTHCONFIG_HPP_
#define _TNMSAUTH_AUTHCONFIG_HPP_

#include <string>


namespace tnmsauth {


#define DEFAULT_TNMSAUTH_TNMSPORT   33701
#define DEFAULT_TNMSAUTH_SOAPPORT   33702
#define DEFAULT_TNMSAUTH_DBNAME     "tcanms"
#define DEFAULT_TNMSAUTH_DBUSER     "tnmsauth"


struct AuthConfig 
{
    std::string  db_host;
    std::string  db_port;
    std::string  db_name;
    std::string  db_user;
    std::string  db_pass;

    time_t       timeout;
    time_t       max_connections;
    int          min_threads;
    int          max_threads;

    uint16_t     tnms_port;
    uint16_t     soap_port;

    AuthConfig()
        : db_host("localhost"),
          db_port(3306),
          db_name(DEFAULT_TNMSAUTH_DBNAME),
          db_user(DEFAULT_TNMSAUTH_DBUSER),
          timeout(120),
          max_connections(100),
          min_threads(1),
          max_threads(75),
          tnms_port(DEFAULT_TNMSAUTH_TNMSPORT),
          soap_port(DEFAULT_TNMSAUTH_SOAPPORT)
    {}
};


} // namespace

#endif  // _TNMSAUTH_AUTHCONFIG_HPP_

