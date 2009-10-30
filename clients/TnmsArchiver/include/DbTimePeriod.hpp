#ifndef _TNMSARCHIVE_ARCHIVETIMEPERIOD_HPP_
#define _TNMSARCHIVE_ARCHIVETIMEPERIOD_HPP_

extern "C" {
#include <time.h>
}

#include <list>
#include <set>
#include <ostream>


namespace tnmsdb {


struct DbTimePeriod 
{
    time_t      start;
    time_t      end;

    DbTimePeriod() : start(0), end(0) {}

    DbTimePeriod ( const DbTimePeriod & tp )
        : start(tp.start), 
          end(tp.end)
    {}

    bool operator< ( const DbTimePeriod & tp ) const
    {
        return start < tp.start; 
    }

    bool operator> ( const DbTimePeriod & tp ) const
    {
        return start > tp.start;
    }

    template <typename T>
    void operator+= ( const T& value )
    {
        start += value;
        end   += value;
    }

    template <typename T>
    void operator-= ( const T& value ) 
    {
        start -= value;
        end   -= value;
    }

    friend std::ostream& operator<< ( std::ostream       & out, 
                                      const DbTimePeriod & tp );
};

typedef std::list<std::string>   DbNameList;
typedef std::list<DbTimePeriod>  DbIndexList;
typedef std::set<DbTimePeriod>   DbPeriodSet;


} // namespace


#endif  //  _TNMSARCHIVE_ARCHIVETIMEPERIOD_HPP_

