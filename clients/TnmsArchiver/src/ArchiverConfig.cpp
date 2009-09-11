#define _TNMSARCHIVE_ARCHIVERCONFIG_CPP_


#include "ArchiverConfig.h"


namespace tnmsarchive {



ArchiverConfig::ArchiverConfig ( const std::string & xmlfilename,
                                 const std::string & rootname )
  : TnmsConfigHandler(xmlfilename, rootname)
{}

ArchiverConfig::ArchiverConfig ( const char * xmlblob, size_t len,
                                 const std::string & rootname )
    : TnmsConfigHandler(xmlblob, len, rootname)
{}


bool
ArchiverConfig::parseClient  ( XmlNode * node )
{
    bool result = false;

    result = TnmsConfigHandler::parseClient(node);

    if ( ! result ) 
        return result;
    
    ArchiveDbConfig  dbConfig;

    XmlNode   * anode = NULL;
    XmlNodeList nlist = node->getNodeList();
    std::string sname = node->getAttribute("name");

    XmlNodeList::iterator  nIter;
    for ( nIter = nlist.begin(); nIter != nlist.end(); ++nIter )
    {
        anode = (XmlNode*) *nIter;

        if ( anode->getNodeName().compare("archiver") == 0 ) {
            SchemaConfig  scfg;

            scfg.index_table = anode->getAttribute("index_name");
            scfg.data_table  = anode->getAttribute("element_data");

            if ( anode->haveAttribute("holddown") )
                scfg.commit_interval_s = StringUtils::fromString<int>(anode->getAttribute("holddown"));
            if ( anode->haveAttribute("table_count") )
                scfg.table_count = StringUtils::fromString<int>(anode->getAttribute("table_count"));
            if ( anode->haveAttribute("flush_interval") )
                scfg.flush_interval = StringUtils::fromString<int>(anode->getAttribute("flush_interval"));

            dbConfig.push_back(scfg);
        }
    }

    dbConfigMap[sname] = dbConfig;

    return true;
}


bool
ArchiverConfig::getDbConfig ( const std::string & name, ArchiveDbConfig & dbConfig )
{
    std::map< std::string, ArchiveDbConfig >::iterator  dIter;

    dIter = dbConfigMap.find(name);

    if ( dIter == dbConfigMap.end() )
        return false;

    dbConfig = dIter->second;

    return true;
}


} // namespace


// _TNMSARCHIVE_ARCHIVERCONFIG_CPP_
