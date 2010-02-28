
#include "FwLogReader.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "StringUtils.h"
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
        std::cout << "Error reading logfile ";
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

        if ( _squeue.size() > 0 )
            _squeue.notify();

        sleep(2);
    } while ( ifs.eof() && ! this->_Alarm );

    ifs.close();

    return;
}

SynchronizedQueue<FwLogEntry>*
FwLogReader::getQueue()
{
    return &this->_squeue;
}


} // namespace

