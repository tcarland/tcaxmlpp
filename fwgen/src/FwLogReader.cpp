
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

    char   line[BIGSTRLINE];

    if ( _tail )
        mode |= std::ios::ate;

    ifs.open(_logfile.c_str(), mode);
    if ( ! ifs ) {
        LogFacility::LogMessage("Error reading logfile: " + _logfile);
        return;
    }

    do 
    {
        while ( ifs.getline(line, BIGSTRLINE) )
        {
            FwLogEntry   fwe;

            if ( FwLogEntry::ParseLogEntry(line, fwe) ) {
                _squeue.push(fwe);
            }
        }

        if ( _squeue.size() > 0 ) {
            _squeue.notify();
        }

        ::usleep(500);
    } while ( ifs.eof() && ! this->_Alarm );

    ifs.close();

    return;
}


FwLogQueue*
FwLogReader::getQueue()
{
    return &this->_squeue;
}


} // namespace

