#ifndef _TNMSARCHIVE_ARCHIVERCONFIG_H_
#define _TNMSARCHIVE_ARCHIVERCONFIG_H_

#include <map>


#include "TnmsConfig.h"

using namespace tnmsdb {


#define  DEFAULT_COMMIT_INTERVAL 300
#define  DEFAULT_DAYS_PER_TABLE  1
#define  DEFAULT_TABLE_COUNT     30
#define  DEFAULT_FLUSH_INTERVAL  12


struct  ArchiveDbConfig
{
    std::string  db_host;
    std::string  db_port;
    std::string  db_name;
    std::string  db_user;
    std::string  db_pass;
};


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



class ArchiverConfigHandler : public TnmsConfigHandler {

  public:

    ArchiverConfigHandler ( const std::string & xmlfilename,
                            const std::string & rootname );

    ArchiverConfigHandler ( const char * xmlblob, size_t len,
                            const std::string & rootname );


    virtual ~ArchiverConfigHandler();


    virtual bool        parseServer  ( XmlNode * node );
    virtual bool        parseClient  ( XmlNode * node );


    bool                getDbConfig  ( const std::string & name, 
                                       SchemaConfigList  & dbConfig );


  protected:

    ArchiverDbMap       dbConfigMap;

};


}  // namespace


#endif  //  _TNMSARCHIVE_ARCHIVERCONFIG_H_

