#define _SOURCE_PGSQLSESSION_CPP_


#include "PgsqlSession.h"

namespace tcasql {


PgsqlSession::PgsqlSession() { }

PgsqlSession::PgsqlSession ( const std::string & dbname, const std::string & dbhost, 
                             const std::string & dbuser, const std::string & dbpass,
                             const std::string & dbport )
{
    _connstr = PgsqlSession::MakeConnStr(dbname, dbhost, dbuser, dbpass, dbport);
}

PgsqlSession::PgsqlSession ( const PgsqlSession & sql )
    : _connstr(sql._connstr)
{
    _dbcon = NULL;
}

PgsqlSession::~PgsqlSession() 
{
    this->dbclose();
}

bool
PgsqlSession::dbconnect()
{
    if ( _connstr.empty() )
        return false;
    
    if ( _dbcon != NULL && _dbcon->is_open() )
        this->dbclose();

    if ( _dbcon == NULL )
        _dbcon = new connection(_connstr);

    return true;
}


bool
PgsqlSession::dbconnect ( const std::string & dbname, const std::string & dbhost, 
                          const std::string & dbuser, const std::string & dbpass,
                          const std::string & dbport )
{
    this->_connstr = PgsqlSession::MakeConnStr(dbname, dbhost, dbuser, dbpass, dbport);
    return this->dbconnect();
}


void
PgsqlSession::dbclose()
{
    if ( _dbcon != NULL ) {
        _dbcon->disconnect();
        delete _dbcon;
        _dbcon = NULL;
    }
    return;
}


bool
PgsqlSession::isConnected()
{
    if ( _dbcon != NULL && _dbcon->is_open() )
        return true;
    return false;
}

const std::string&
PgsqlSession::sqlErrorStr()
{
    return _sqlerr;
}


/*  PgsqlSession methods */



template<typename TRANSACTOR>
bool
PgsqlSession::run ( const TRANSACTOR & transaction ) const
{
    try {

        _dbcon->perform(transaction);

    } catch ( const sql_error & e ) {
        std::ostringstream  msg;
        msg << "SQL Error: trying to execute transactor:\n"
            << "\tquery : " << e.query() << "\n"
            << "\tmessage : " << e.what();
        _sqlerr = msg.str();
        return false;
    }

    return true;
}


std::string
PgsqlSession::MakeConnStr ( const std::string & dbname, const std::string & dbhost, 
                            const std::string & dbuser, const std::string & dbpass,
                            const std::string & dbport )
{
    std::string connstr = "";

    if ( ! dbname.empty() )
        connstr += (" dbname=" + dbname);
    if ( ! dbhost.empty() )
        connstr += (" host=" + dbhost);
    if ( ! dbuser.empty() )
        connstr += (" user=" + dbuser);
    if ( ! dbpass.empty() )
        connstr += (" password=" + dbpass);
    if ( ! dbport.empty() )
        connstr += (" port=" + dbport);

    return connstr;
}


} // namespace

//  _SOURCE_PGSQLSESSION_CPP_
