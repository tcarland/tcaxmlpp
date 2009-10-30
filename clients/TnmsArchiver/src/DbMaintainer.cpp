#define _DBMAINTAINER_CPP_

#include "DbMaintainer.h"

#include "StringUtils.h"
#include "LogFacility.h"
using namespace tcanetpp;


namespace tnmsdb {


DbMaintainer::DbMaintainer ( const std::string & index_table,
                             const std::string & data_table,
                             int table_count )
    : _indexName(index_table),
      _dataName(data_table),
      _numTables(table_count)
{
}

DbMaintainer::~DbMaintainer()
{
}

void
DbMaintainer::runMaintainer ( const time_t & now )
{
    IndexList  to_create, to_delete;

    LogFacility::LogMessage("DbMaintainer::run()");

    this->getCurrentPeriods(std::inserter(_current, _current.begin()));
    this->getDatabasePeriods(std::inserter(_dbperiods, _dbperiods.begin()));

    this->createTimePeriods(to_create);
    this->deleteTimePeriods(to_delete);

    _current.clear();
    _dbperiods.clear();

    return;
}


std::string
DbMaintainer::getTargetTable ( const time_t & timestamp )
{
    std::string target;

    DbTimePeriod  tp = this->getTargetTimePeriod(timestamp);
    target = _dataName + "_" + StringUtils::toString(tp.start);

    return target;
}


DbTimePeriod 
DbMaintainer::getTargetTimePeriod ( const time_t & timestamp )
{
    time_t  t = timestamp;

    if ( timestamp == 0 )
        t = ::time(NULL);

    return(this->getInterval(t));
}


DbTimePeriod
DbMaintainer::getInterval ( const time_t & timestamp )
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
DbMaintainer::getCurrentPeriods ( OutputIterator_ out )
{
    time_t       now = ::time(NULL);
    DbTimePeriod current_end(this->getCurrentEnd(now));

    this->slideByDays(current_end, 1);
    this->getPeriodsInRange(this->getCurrentBegin(now), current_end, out);

    return;
}


template<typename OutputIterator_>
void
DbMaintainer::getDatabasePeriods ( OutputIterator_ out )
{
    NameList  index_names;

    this->getTimePeriods(index_names);

    for ( NameList::iterator i = index_names.begin(); i != index_names.end(); ++i ) {
        std::string   time;
        DbTimePeriod  index;

        std::string & name  = (*i);

        time  = name.substr(name.rfind('_') + 1);
        index = this->getInterval(StringUtils::fromString<time_t>(time));
  
        *out++ = index;
    }

    return;
}   


void
DbMaintainer::slideByDays ( DbTimePeriod & period, int days )
{
    struct tm  start_tm, end_tm;

    ::localtime_r(&period.start, &start_tm);
    ::localtime_r(&period.end, &end_tm);

    start_tm.tm_mday += days;
    end_tm.tm_mday   += days;

    start_tm.tm_isdst = end_tm.tm_isdst = -1;

    period.start      = ::mktime( &start_tm );
    period.end        = ::mktime( &end_tm );

    return;
}


template <typename OutputIterator_>
void
DbMaintainer::getPeriodsInRange ( const DbTimePeriod & begin,
                                         const DbTimePeriod & end, 
                                         OutputIterator_      out )
{
    DbTimePeriod  current(begin);

    while ( current.start <= end.start ) 
    {
        *out++ = current;
        this->slideByDays( current, 1 );
    }

    return;
}


DbTimePeriod
DbMaintainer::getCurrentBegin ( const time_t & timestamp )
{
    time_t     then_t;
    struct tm  now_tm;
    int        size;

    size = ( _numTables > 0 ) ? _numTables : 1;

    ::localtime_r(&timestamp, &now_tm);
    now_tm.tm_mday -= size;
    now_tm.tm_isdst = -1;
    then_t = ::mktime(&now_tm);
  
    return this->getInterval(then_t);
}

 
DbTimePeriod 
DbMaintainer::getCurrentEnd ( const time_t & timestamp )
{
    return this->getInterval(timestamp);
}


template<typename OutputIterator_>
void
DbMaintainer::getIntervalsToCreate ( OutputIterator_ out )
{
    std::set_difference(
        _current.begin(), _current.end(),
        _dbperiods.begin(), _dbperiods.end(), out
    );
    return;
}


template <typename OutputIterator_>
void
DbMaintainer::getIntervalsToDelete ( OutputIterator_ out )
{
    if ( _numTables > 0 ) 
    {
        std::set_difference( 
            _dbperiods.begin(), _dbperiods.end(),
            _current.begin(), _current.end(), out
        );
    }
    return;
}


std::ostream &
operator<< ( std::ostream & out, const DbTimePeriod & t )
{
    std::string start_str(ctime(&t.start));
    std::string end_str(ctime(&t.end));

    return out << "[ " << start_str << "/" << end_str << "]" << std::endl;
}




} // namespace

// _DBMAINTAINER_CPP_
