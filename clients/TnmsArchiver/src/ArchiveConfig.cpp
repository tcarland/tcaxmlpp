#define _TNMSARCHIVE_ARCHIVECONFIG_CPP_

#include "ArchiveConfig.h"


namespace tnmsdb {



ArchiveConfigHandler::ArchiveConfigHandler ( const std::string & xmlfilename,
                                 const std::string & rootname )
  : TnmsConfigHandler(xmlfilename, rootname)
{}


ArchiveConfigHandler::ArchiveConfigHandler ( const char * xmlblob, size_t len,
                                 const std::string & rootname )
    : TnmsConfigHandler(xmlblob, len, rootname)
{}


bool
ArchiveConfigHandler::parse()
{
    return(TnmsConfigHandler::parse());
}


bool
ArchiveConfigHandler::parseClient  ( XmlNode * node )
{
    bool result = false;

    result = TnmsConfigHandler::parseClient(node);

    if ( ! result ) 
        return result;
    
    SchemaConfigList  dbConfig;

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
                scfg.tables_to_keep = StringUtils::fromString<int>(anode->getAttribute("table_count"));
            if ( anode->haveAttribute("flush_interval") )
                scfg.flush_interval = StringUtils::fromString<int>(anode->getAttribute("flush_interval"));

            dbConfig.push_back(scfg);
        }
    }

    dbConfigMap[sname] = dbConfig;

    return true;
}


bool
ArchiveConfigHandler::getDbConfig ( const std::string & name, SchemaConfigList & dbConfig )
{
    ArchiverDbMap::iterator  dIter;

    dIter = dbConfigMap.find(name);

    if ( dIter == dbConfigMap.end() )
        return false;

    dbConfig = dIter->second;

    return true;
}


} // namespace


// _TNMSARCHIVE_ARCHIVERCONFIG_CPP_
