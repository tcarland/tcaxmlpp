#ifndef _TNMSARCHIVE_ARCHIVECONFIG_H_
#define _TNMSARCHIVE_ARCHIVECONFIG_H_

#include <map>

#include "TnmsConfig.h"
using namespace tnmsCore;



namespace tnmsdb {


#define  DEFAULT_COMMIT_INTERVAL 300
#define  DEFAULT_DAYS_PER_TABLE  1
#define  DEFAULT_TABLE_COUNT     30
#define  DEFAULT_FLUSH_INTERVAL  12


/** Database Settings */
struct  ArchiveDbConfig
{
    std::string  db_name;
    std::string  db_user;
    std::string  db_pass;
    std::string  db_host;
    std::string  db_port;
};


/** Schema specific configuration. */
struct  SchemaConfig
{
    std::string    index_table;
    std::string    data_table;

    int            commit_interval_s;
    int            tables_to_keep;
    int            flush_interval;
    bool           cache_index;

    SchemaConfig()
        : commit_interval_s(DEFAULT_COMMIT_INTERVAL),
          tables_to_keep(DEFAULT_TABLE_COUNT),
          flush_interval(DEFAULT_FLUSH_INTERVAL),
          cache_index(true)
    {}
};



typedef std::list< SchemaConfig >                 SchemaConfigList;
typedef std::map< std::string, SchemaConfigList > ArchiverDbMap;



class ArchiveConfigHandler : public TnmsConfigHandler {

  public:

    ArchiveConfigHandler ( const std::string & xmlfilename,
                           const std::string & rootname );

    ArchiveConfigHandler ( const char * xmlblob, size_t len,
                           const std::string & rootname );


    virtual ~ArchiveConfigHandler();

    virtual bool        parse();
    virtual bool        parseServer  ( XmlNode * node );
    virtual bool        parseClient  ( XmlNode * node );


    bool                getDbConfig  ( const std::string & name, 
                                       SchemaConfigList  & dbConfig );


  public:

    ArchiverDbMap       dbConfigMap;

};


}  // namespace


#endif  //  _TNMSARCHIVE_ARCHIVECONFIG_H_

