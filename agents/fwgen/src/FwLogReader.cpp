
#include "FwLogReader.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "StringUtils.h"
#include "LogFacility.h"
using namespace tcanetpp;


namespace fwgen {



FwLogReader::FwLogReader ( const std::string & logfile, bool tail )
    : _logfile(logfile), _tail(tail)
{}

void
FwLogReader::run()
{
    std::ifstream        ifs;
    std::ios::openmode   mode = std::ios::in;

    char   line[TCANET_BIGSTRLINE];

    if ( _tail )
        mode |= std::ios::ate;

    ifs.open(_logfile.c_str(), mode);

    if ( ! ifs ) {
        LogFacility::LogMessage("Error reading logfile: " + _logfile);
        return;
    }

    LogFacility::LogMessage("FwLogReader started for logfile: " + _logfile);

    if ( ! _match.empty() )
        FwLogEntry::SetDefaultMatch(_match);

    do 
    {
        while ( ifs.getline(line, TCANET_BIGSTRLINE) )
        {
            FwLogEntry   fwe;

            if ( FwLogEntry::ParseLogEntry(line, fwe) )
                _squeue.push(fwe);
        }

        if ( _squeue.size() > 0 )
            _squeue.notify();

        ::usleep(5000000);

        if ( ifs.eof() && ! _tail )
            this->_Alarm = true;

        if ( ifs.eof() )
            ifs.clear();

    } while ( ! this->_Alarm );

    LogFacility::LogMessage("FwLogReader thread terminating..");
    ifs.close();

    return;
}


FwLogQueue*
FwLogReader::getQueue()
{
    return &this->_squeue;
}
void
FwLogReader::setMatch ( const std::string & match )
{
    _match = match;
}


} // namespace

