#ifndef _TNMSAUTH_AUTHDBCONFIG_HPP_
#define _TNMSAUTH_AUTHDBCONFIG_HPP_

#include <string>


namespace tnmsauth {


#define DEFAULT_TNMSAUTH_TNMSPORT   33701
#define DEFAULT_TNMSAUTH_SOAPPORT   33702
#define DEFAULT_TNMSAUTH_DBNAME     "tnms"
#define DEFAULT_TNMSAUTH_DBUSER     "tnmsauth"


struct AuthDbConfig 
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

    AuthDbConfig()
        : db_host("localhost"),
          db_port("3306"),
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


struct TnmsDbUser {

    uint32_t            uid;
    uint32_t            gid;
    uint32_t            auth_id;
    std::string         username;
    std::string         ticket;
    std::string         ipaddr;
    std::string         auth_method;
    std::string         auth_bin;
    bool                internal;
    time_t              last;

    TnmsDbUser() :
        uid(0), gid(0), auth_id(0),
        internal(false), last(0)
    {}
};


struct TnmsDbAgent {
    uint32_t            agent_id;
    uint32_t            gid;
    std::string         agent_name;
    std::string         agent_config;
    std::string         ipaddr;
    time_t              last;

    TnmsDbAgent()
        : agent_id(0), gid(0), last(0)
    {}
};



} // namespace

#endif  // _TNMSAUTH_AUTHDBCONFIG_HPP_

