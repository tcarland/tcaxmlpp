#ifndef _TNMSAUTH_AUTHDBCONFIG_HPP_
#define _TNMSAUTH_AUTHDBCONFIG_HPP_

#include <string>
#include <list>


namespace tnmsauth {


#define DEFAULT_TNMSAUTH_TNMSPORT   33701
#define DEFAULT_TNMSAUTH_SOAPPORT   33801
#define DEFAULT_TNMSAUTH_DBNAME     "tnms"
#define DEFAULT_TNMSAUTH_DBUSER     "tnmsauth"

#define AUTHDB_METHOD_DBSTATIC      "dbstatic"
#define AUTHDB_REFRESH_TRIGGER      "REFRESH_TRIGGER"


typedef std::list<std::string>  StringList;


struct AuthDbConfig  {
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


struct TnmsDbFilter {
    uint32_t            gid;
    StringList          authorizations;
    bool                isInclude;

    TnmsDbFilter() 
        : gid(0), isInclude(true) 
    {}
    TnmsDbFilter ( uint32_t id )
        : gid(id), isInclude(true) 
    {}
};



struct TnmsDbUser {
    std::string         username;
    std::string         ticket;
    std::string         ipaddr;
    std::string         config;

    uint32_t            uid;
    uint32_t            gid;

    uint32_t            authtype_id;
    std::string         auth_method;
    std::string         auth_bin;

    bool                internal;
    bool                isAgent;
    time_t              last;

    TnmsDbUser() :
        uid(0), gid(0), authtype_id(0),
        internal(false), isAgent(false), last(0)
    {}

    TnmsDbUser ( const std::string & username_ ) :
        username(username_),
        uid(0), gid(0), authtype_id(0),
        internal(false), isAgent(false), last(0)
    {}

};


} // namespace

#endif  // _TNMSAUTH_AUTHDBCONFIG_HPP_

