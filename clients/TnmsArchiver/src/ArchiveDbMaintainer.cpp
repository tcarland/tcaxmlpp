#define _ARCHIVEMAINTAINER_CPP_

#include "ArchiveDbMaintainer.h"

namespace tnmsdb {


ArchiveDbMaintainer::ArchiveDbMaintainer()
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
    TimePeriod  foo;

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
