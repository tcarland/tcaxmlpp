#define _TNMSDB_ARCHIVER_CPP_

#include "Archiver.h"

#include "ArchiveClient.h"
#include "ArchiveSubscriber.h"


namespace tnmsdb {


Archiver::Archiver ( SqlSession * session, SchemaConfig & config )
    : ArchiveDbMaintainer(config.index_table, config.data_table, 1, config.tables_to_keep),
      sql(new SqlSession(*session)),
      tree(new TnmsTree()),
      notifier(new ArchiveSubscriber()),
      schema(config)
{
    tree->subscribe("*", notifier);
}


Archiver::~Archiver()
{
    delete tree;
}


void
Archiver::runUpdates ( const time_t & now, bool flush )
{
    if ( ! notifier->lock() )
        return;

    while ( ! notifier->metricq.empty() )
    {
        TnmsMetric & metric = notifier->metricq.front();
        Query        query  = sql->newQuery();

        uint32_t    id     = this->getElementId(metric.getElementName());
        const std::string & valstr = metric.getValue();

        query << "INSERT DELAYED INTO " << tableName 
              << " (element_id, value_type, value, value_avg, samples, value_str, data, last ) VALUES ("
              << id << ", " << metric.getValueType() << ", " << metric.getValue() << ", "
              << metric.getValueAvg() << ", " << metric.getSamples() << ", \"" << valstr << "\", \"" 
              << metric.getPvtData() << "\", " << metric.getTimestamp() << ")";

        if ( ! this->submitQuery(query) )
        {
            LogFacility::Message  msg;
            msg << "Archiver::runUpdates() Error in query submit: '" << sql->getErrorStr() << "'";
            LogFacility::LogMessage(msg.str());
        }

        _metricq.pop();
    }

    return;
}

void
Archiver::loadElementMap()
{
    Query  query = sql->newQuery();
    Result res;
    Row    row;

    query << "SELECT id, name FROM " << config.index_table;

    if ( ! this->submitQuery(query, result) )
        return;

    Result::iterator  rIter;

    for ( rIter = res.begin(); rIter != res.end(); ++rIter )
    {
        uint32_t     id;
        std::string  name;

        row  = (Row) *rIter;
        id   = StringUtils::fromString<uint32_t>(row[0]);
        name = std::string(row[1]);

        _elementMap[name] = id;
    }

    return;
}


uint32_t
Archiver::getElementId ( const std::string & name )
{
    uint32_t  id = 0;

    ElementMap::iterator  eIter = _elementMap.find(name);

    if ( eIter == _elementMap.end() )
        return this->queryElementId(name);

    id = eIter->second;

    return id;
}


uint32_t
Archiver::queryElementId ( const std::string & name )
{
    uint32_t  id = 0;

    Query  query;
    Result res;
    Row    row;

    query << "SELECT id FROM " << config.index_table << " WHERE name=\""
          << name << "\"";

    if ( ! this->submitQuery(query, res) )
        return id;
    if ( res.size() == 0 )
        return this->insertElementId(name);

    Result::iterator rIter;

    rIter = res.begin();
    row   = (Row) *rIter;
    id    = StringUtils::fromString<uint32_t>(row[0]);

    _elementMap[name] = id;

    return id;
}


uint32_t
Archiver::insertElement ( const std::string & name )
{
    uint32_t id = 0;

    Query query;

    query << "INSERT INTO " << config.index_table << " ( name ) VALUES ( \"" << name << "\" )";

    if ( ! this->submitQuery(query) )
        return id;

    id = this->insert_id();

    return id;
}


}  // namespace 


//  _TNMSDB_ARCHIVER_CPP_

