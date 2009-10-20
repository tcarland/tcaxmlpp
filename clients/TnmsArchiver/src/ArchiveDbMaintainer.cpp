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
    IndexList  to_create, to_delete;

    LogFacility::LogMessage("ArchiveDbMaintainer::run()");

    this->getCurrentPeriods(std::inserter(_current, _current.begin()));
    this->getDbPeriods(std::inserter(_dbperiods, _dbperiods.begin()));

    this->createTimePeriods(to_create);
    this->deleteTimePeriods(to_delete);

    _current.clear();
    _dbperiods.clear();

    return;
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

/*
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
*/

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


template<typename OutputIterator_>
void
ArchiveDbMaintainer::getCurrentPeriods ( OutputIterator_ out )
{
    time_t       now = ::time(NULL);
    time_period  current_end(this->getCurrentEnd(now));

    this->slideByDays(current_end, 1);
    this->getPeriodsInRange(this->getCurrentBegin(now), current_end, out);
}


template<typename OutputIterator_>
void
ArchiveDbMaintainer::getDatabasePeriods ( OutputIterator_ out ) 
{
    NameList  index_names;

    this->getTimePeriods(index_names);

    for ( NameList::iterator i = index_names.begin(); i != index_names.end(); ++i ) {
        std::string   time;
        time_period   index;

        std::string & name  = (*i);

        time  = name.substr(name.rfind('_') + 1);
        index = this->getContainingInterval(StringConversion::fromString<time_t>(time));
  
        *out++ = index;
    }
}   


void
ArchiveDbMaintainer::slideByDays ( time_period & period, int days )
{
    struct tm  start_tm, end_tm;

    ::localtime_r(&period.start, &start_tm);
    ::localtime_r(&period.end, &end_tm);

    start_tm.tm_mday += days;
    end_tm.tm_mday   += days;

    // force mktime to behave properly.
    start_tm.tm_isdst = end_tm.tm_isdst = -1;
    
    period.start  = ::mktime( &start_tm );
    period.end    = ::mktime( &end_tm );

    return;
}


template <typename OutputIterator_>
void
ArchiveDbMaintainer::getPeriodsInRange ( const time_period & begin, 
                                         const time_period & end, 
                                         OutputIterator_     out )
{
    time_period  current(begin);

    while ( current.start <= end.start ) 
    {
        *out++ = current;
        this->slideByDays( current, 1 );
    }
}


time_period
ArchiveDbMaintainer::getCurrentBegin ( const time_t & timestamp )
{
    time_t     then_t;
    struct tm  now_tm;
    int        size;

    size = ( _table_count > 0 ) ? _table_count : 1;

    ::localtime_r(&timestamp, &now_tm);
    now_tm.tm_mday -= size;
    now_tm.tm_isdst = -1;
    then_t = ::mktime(&now_tm);
  
    return getContainingInterval(then_t);
}

 
time_period 
ArchiveDbMaintainer::getCurrentEnd ( const time_t & timestamp )
{
    return this->getContainingInterval(timestamp);
}


template<typename OutputIterator_>
void
ArchiveDbMaintainer::getIntervalsToCreate ( OutputIterator_ out )
{
    std::set_difference(
        _current_periods.begin(), _current_periods.end(),
        _database_periods.begin(), _database_periods.end(), out
    );
}


template <typename OutputIterator_>
void
ArchiveDbMaintainer::getIntervalsToDelete ( OutputIterator_ out )
{
    if ( _table_count > 0 ) 
    {
        std::set_difference( 
            _database_periods.begin(), _database_periods.end(),
            _current_periods.begin(), _current_periods.end(), out
        );
    }
}


std::ostream &
operator<< ( std::ostream & out, const time_period & t )
{
    std::string start_str(ctime(&t.start));
    std::string end_str(ctime(&t.end));

    return out << "[ " << start_str << "/" << end_str << "]" << std::endl;
}




} // namespace

// _ARCHIVEMAINTAINER_CPP_
