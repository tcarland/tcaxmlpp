#ifndef _TNMSAUTH_AUTHCONFIG_HPP_
#define _TNMSAUTH_AUTHCONFIG_HPP_

#include <string>


namespace tnmsauth {


#define DEFAULT_TNMSAUTHD_PORT   33701
#define DEFAULT_SOAPAUTHD_PORT   33702
#define DEFAULT_TNMSAUTHDB_NAME  "tcanms"
#define DEFAULT_TNMSAUTH_USER    "tnmsauth"


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
          db_port(DEFAULT_TNMSAUTHD_PORT),
          db_name(DEFAULT_TNMSAUTHDB_NAME),
          db_user(DEFAULT_TNMSAUTH_USER),
          timeout(120),
          max_connections(100),
          min_threads(1),
          max_threads(75),
          tnms_port(DEFAULT_TNMSAUTHD_PORT),
          soap_port(DEFAULT_SOAPAUTHD_PORT)
    {}
};


} // namespace

#endif  // _TNMSAUTH_AUTHCONFIG_HPP_

