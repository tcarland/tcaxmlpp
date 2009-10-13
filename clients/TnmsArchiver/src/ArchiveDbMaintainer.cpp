#define _ARCHIVEMAINTAINER_CPP_

#include "ArchiveDbMaintainer.h"

namespace tnmsdb {


ArchiveDbMaintainer::ArchiveDbMaintainer ( const std::string & index_table,
                                           const std::string & data_table,
                                           int days_per_interval,
                                           int table_count )
    : _index(index_table),
      _data(data_table),
      _numDays(days_per_interval),
      _numTables(table_count)
{
}

ArchiveDbMaintainer::~ArchiveDbMaintainer()
{
}

void
ArchiveDbMaintainer::run()
{
}


std::string
ArchiveDbMaintainer::getTargetTable ( const time_t & timestamp )
{
    std::string target;

    return target;
}


DbTimePeriod 
ArchiveDbMaintainer::getTargetTimePeriod ( const time_t & timestamp )
{
    DbTimePeriod  foo;

    return foo;
}


void       
ArchiveDbMaintainer::getTimePeriods ( NameList & nameList )
{
}


void       
ArchiveDbMaintainer::createTimePeriods ( IndexList & indices )
{
}

void       
ArchiveDbMaintainer::deleteTimePeriods ( IndexList & indices )
{
}



} // namespace

// _ARCHIVEMAINTAINER_CPP_
