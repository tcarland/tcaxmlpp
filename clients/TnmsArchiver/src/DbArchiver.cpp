#define _TNMSDB_ARCHIVER_CPP_

#include "DbArchiver.h"

#include "ArchiveClient.h"
#include "ArchiveSubscriber.h"

#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsdb {


DbArchiver::DbArchiver ( SqlSession * session, SchemaConfig & config )
    : DbMaintainer(config.index_table, config.data_table, config.tables_to_keep),
      sql(new SqlSession(*session)),
      tree(new TnmsTree()),
      notifier(new ArchiveSubscriber()),
      schema(config)
{
    tree->subscribe("*", notifier);
}


DbArchiver::~DbArchiver()
{
    delete tree;
}


void
DbArchiver::runUpdates ( const time_t & now, bool flush )
{
    if ( ! sql->isConnected() ) 
    {
        LogFacility::LogMessage("DbArchiver intitiating database connection");
        if ( ! sql->connect() ) {
            LogFacility::LogMessage("DbArchiver: ERROR in connection: " 
                + sql->sqlErrorStr());
            return;
        }
        LogFacility::LogMessage("Archive connection established for " 
            + schema.index_table);
    }

    if ( ! notifier->lock() )
        return;

    std::string table_name = this->getTargetTable(now);

    while ( ! notifier->metricq.empty() )
    {
        TnmsMetric & metric = notifier->metricq.front();
        Query        query  = sql->newQuery();

        const std::string & valstr = metric.getValue();
        uint32_t    id     = this->getElementId(metric.getElementName());
        uint64_t    val    = metric.getValue<uint64_t>();
        uint64_t    avg    = metric.getValueAvg<uint64_t>();

        if ( id == 0 )
            continue;

        query << "INSERT DELAYED INTO " << table_name 
              << " (element_id, value_type, value, value_avg, samples, value_str, data, last ) VALUES ("
              << id << ", " << metric.getValueType() << ", " << val << ", " << avg << ", " 
              << metric.getSamples() << ", \"" << valstr << "\", \"" << metric.getPvtData() << "\", " 
              << metric.getTimestamp() << ")";

        if ( ! sql->submitQuery(query) ) {
            LogFacility::LogMessage("DbArchiver::runUpdates() ERROR in query submit: " + sql->sqlErrorStr());
            break;
        }

        notifier->metricq.pop();
    }

    return;
}

void
DbArchiver::loadIndexMap()
{
    Query  query = sql->newQuery();
    Result res;
    Row    row;

    query << "SELECT id, name FROM " << schema.index_table;

    if ( ! sql->submitQuery(query, res) )
        return;

    Result::iterator  rIter;

    for ( rIter = res.begin(); rIter != res.end(); ++rIter )
    {
        uint32_t     id;
        std::string  name;

        row  = (Row) *rIter;
        id   = StringUtils::fromString<uint32_t>(row[0]);
        name = std::string(row[1]);

        _indexes[name] = id;
    }

    return;
}


uint32_t
DbArchiver::getElementId ( const std::string & name )
{
    uint32_t  id = 0;

    IndexMap::iterator  eIter = _indexes.find(name);

    if ( eIter == _indexes.end() )
        return this->queryElementId(name);

    id = eIter->second;

    return id;
}


uint32_t
DbArchiver::queryElementId ( const std::string & name )
{
    uint32_t  id = 0;

    Query  query;
    Result res;
    Row    row;

    query << "SELECT id FROM " << schema.index_table << " WHERE name=\""
          << name << "\"";

    if ( ! sql->submitQuery(query, res) )
        return id;
    if ( res.size() == 0 )
        return this->insertElementId(name);

    Result::iterator rIter;

    rIter = res.begin();
    row   = (Row) *rIter;
    id    = StringUtils::fromString<uint32_t>(row[0]);

    _indexes[name] = id;

    return id;
}


uint32_t
DbArchiver::insertElementId ( const std::string & name )
{
    Query    query;
    uint32_t id = 0;

    query << "INSERT INTO " << schema.index_table << " ( name ) VALUES ( \"" << name << "\" )";

    if ( ! sql->submitQuery(query) )
        return id;
 
    // this is mysql specific
    id = sql->insert_id();

    if ( id > 0 )
        _indexes[name] = id;

    return id;
}

void       
DbArchiver::getTimePeriods ( DbNameList & namelist )
{
    Query   query;
    Result  res;
    Row     row;

    query << "SHOW TABLES";

    if ( ! sql->submitQuery(query, res) )
        return;

    Result::iterator  rIter;

    for ( rIter = res.begin(); rIter != res.end(); ++rIter )
    {
        std::string  name;

        row  = (Row) *rIter;
        name = std::string(row[0]);

        if ( StringUtils::indexOf(name, "_") <= 0 || name.compare(schema.data_table) == 0 )
            continue;

        if ( StringUtils::indexOf(name, schema.data_table) >= 0 )
            namelist.push_back(name);
    }

    return;
}


void       
DbArchiver::createTimePeriods ( DbIndexList & indices )
{
    DbIndexList::const_iterator iIter;

    for ( iIter = indices.begin(); iIter != indices.end(); ++iIter ) {
        const DbTimePeriod & period = *iIter;

        std::string  tpstr  = StringUtils::toString(period.start);
        std::string  target = std::string(schema.data_table + '_' + tpstr);
        Query        query;

        query << "CREATE TABLE " << target << " LIKE " << schema.data_table;

        sql->submitQuery(query);
    }

    return;
}


void       
DbArchiver::deleteTimePeriods ( DbIndexList & indices )
{    
    DbIndexList::const_iterator  iIter;

    for ( iIter = indices.begin(); iIter != indices.end(); ++iIter ) 
    {
        const DbTimePeriod & period = *iIter;
        std::string          target = schema.data_table;
        Query                query;

        target.append("_");
        target.append(StringUtils::toString(period.start));

        query << "DROP TABLE " << target;

        if ( LogFacility::GetDebug() )
            LogFacility::LogMessage(query.preview());

        if ( ! sql->submitQuery(query) )
            LogFacility::LogMessage("Failed to drop table " + target);
        else
            LogFacility::LogMessage("Dropped table " + target);
    }

    return;
}


}  // namespace 


//  _TNMSDB_DBARCHIVER_CPP_

