#define _ARCHIVEMAINTAINER_CPP_

#include "ArchiveDbMaintainer.h"

#include "StringUtils.h"
using namespace tcanetpp;


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

    DbTimePeriod  tp = this->getTargetTimePeriod(timestamp);
    target = _data + "_" + StringUtils::toString(tp.start);

    return target;
}


DbTimePeriod 
ArchiveDbMaintainer::getTargetTimePeriod ( const time_t & timestamp )
{
    DbTimePeriod  tp;

    time_t  t = timestamp;

    if ( timestamp == 0 )
        t = ::time(NULL);

    tp = this->getInterval(t);

    return tp;
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


DbTimePeriod
ArchiveDbMaintainer::getInterval ( const time_t & timestamp )
{
    struct tm     start, end;
    DbTimePeriod  tp;

    ::localtime_r(&timestamp, &start);

    end.tm_isdst = start.tm_isdst = -1;
    end.tm_hour  = start.tm_hour = 0;
    end.tm_min   = start.tm_min  = 0;
    end.tm_sec   = start.tm_sec  = 0;
    end.tm_year  = start.tm_year;
    end.tm_mon   = start.tm_mon;
    end.tm_mday  = start.tm_mday + 1;

    tp.start     = ::mktime(&start);
    tp.end       = ::mktime(&end);

    return tp;
}

} // namespace

// _ARCHIVEMAINTAINER_CPP_
