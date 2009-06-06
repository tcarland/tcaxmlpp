#define _TNMSAUTH_TNMSAUTHDMANAGER_CPP_

#include "TnmsAuthdManager.h"


namespace tnmsauth {


TnmsAuthdManager::TnmsAuthdManager() 
{}


TnmsAuthdManager::~TnmsAuthdManager() 
{}


void
TnmsAuthdManager::timeout ( const EventTimer * timer ) 
{}


void
TnmsAuthdManager::run()
{
    return;
}


void
TnmsAuthdManager::setAlarm()
{
    this->_alarm = true;
}


void
TnmsAuthdManager::setHUP()
{
    this->_HUP = true;
}


void
TnmsAuthdManager::setDebug ( bool d )
{
    _debug = d;
}


std::string
TnmsAuthdManager::getErrorStr()
{
    return this->_errstr;
}


bool
TnmsAuthdManager::parseConfig ( const std::string & cfg, const time_t & now )
{}


void
TnmsAuthdManager::logRotate ( std::string logfile, const time_t & now )
{
    if ( logfile.empty() ) 
        return;

    char        datestr[64];
    struct tm  *ltm   = ::localtime(&now);
    int         today = ltm->tm_yday;

    if ( _today != today ) {
        ::strftime(datestr, 64, ".%Y%02m%02d", ltm);
        logfile.append(datestr);
        LogFacility::OpenLogFile(LogFacility::GetLogPrefix(), logfile, true);
        _today = today;
    }

    _logRotate = now + LOG_ROTATE_INTERVAL;

    return;
}




}  // namespace


// _TNMSAUTH_TNMSAUTHDMANAGER_CPP_

