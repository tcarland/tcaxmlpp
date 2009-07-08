/**  MysqlSession
  *   sqlplus wrapper to an SQL session
  *
  *  @Author   tcarland.gmail.com
  *  @Version  0.5
 **/
#define _SOURCE_MYSQLSESSION_CPP_

extern "C" {
# include <unistd.h>
# include <stdio.h>
}

#include "MysqlSession.h"


namespace tcasql {



MysqlSession::MysqlSession() throw ( SqlException )
    : _dbcon(NULL),
      _timeout(DEFAULT_CONNECT_TIMEOUT),
      _compress(false),
      _connected(false)
{
    this->init();
}


MysqlSession::MysqlSession ( const std::string & dbname, const std::string & dbhost,
			     const std::string & dbuser, const std::string & dbpass,
                             const std::string & dbport )
    throw ( SqlException )
    : _dbcon(NULL),
      _dbname(dbname),
      _dbhost(dbhost),
      _dbuser(dbuser),
      _dbpass(dbpass),
      _dbport(dbport),
      _timeout(DEFAULT_CONNECT_TIMEOUT),
      _compress(false),
      _connected(false)
{
    this->init();
}


MysqlSession::MysqlSession ( const MysqlSession & sql ) throw ( SqlException )
    : _dbcon(NULL),
      _dbname(sql._dbname),
      _dbhost(sql._dbhost),
      _dbuser(sql._dbuser),
      _dbpass(sql._dbpass),
      _dbport(sql._dbport),
      _timeout(sql._timeout),
      _compress(false),
      _connected(false)
{
    this->init();
}


MysqlSession::~MysqlSession()
{
    if ( _dbcon )
        this->dbclose();
}

void
MysqlSession::init() throw ( SqlException )
{
    if ( _dbcon ) {
        this->dbclose();
    }
    
    _dbcon = mysql_init(NULL);
    
    if ( _dbcon == NULL ) {
        _sqlerr = "MysqlSession::init(): mysql_init() failed";
        throw SqlException(_sqlerr);
    }

    return;
}


bool
MysqlSession::dbconnect ( const std::string & dbname, const std::string & dbhost,
                          const std::string & dbuser, const std::string & dbpass,
                          const std::string & dbport )
    throw ( SqlException )
{
    if ( this->isConnected() ) {
        this->dbclose();
    }

    _dbname = dbname;
    _dbhost = dbhost;
    _dbuser = dbuser;
    _dbpass = dbpass;
    _dbport = dbport;

    return this->dbconnect();
}


bool
MysqlSession::dbconnect()
    throw ( SqlException )
{
    MYSQL * conn     = NULL;
    u_int   port     = 0;

    if ( _dbuser.empty() ) {
        _sqlerr = "MysqlSession::dbconnect(): invalid connection parameters";
        return false;
    }

    if ( this->isConnected() )
        this->dbclose();

    if ( _dbcon == NULL )
        this->init();

    if ( _compress )
        mysql_options(_dbcon, MYSQL_OPT_COMPRESS, 0);

    if ( _timeout ) {
        std::string timeout = MysqlSession::toString(_timeout);
        mysql_options(_dbcon, MYSQL_OPT_CONNECT_TIMEOUT, timeout.c_str());
    }

    if ( ! _dbport.empty() )
        port = MysqlSession::fromString<u_int>(_dbport);

    conn = mysql_real_connect(_dbcon, _dbhost.c_str(), _dbuser.c_str(), 
            _dbpass.c_str(), _dbname.c_str(), 0, NULL, 0);
    
    if ( conn == NULL ) {
        _sqlerr = "MysqlSession::dbconnect(): ";
        _sqlerr.append((std::string) mysql_error(_dbcon));
        _connected  = false;
        return false;
    } else
        _connected = true;

    return this->isConnected();
}


void
MysqlSession::dbclose()
{
    if ( ! this->isConnected() ) {
        _sqlerr = "MysqlSession::dbclose(): No connection exists.";
	return;
    }

    mysql_close(_dbcon);
    _dbcon  = NULL;
    _sqlerr = "MysqlSession::dbclose()";

    return;
}


bool
MysqlSession::isConnected()
{
    if ( _dbcon == NULL || ! _connected )
	return false;

    if ( mysql_ping(_dbcon) != 0 )
        _connected = false;
    else 
        _connected = true;

    return _connected;
}


Query
MysqlSession::newQuery()
{
    Query  q;
    return q;
}


bool
MysqlSession::submitQuery ( const std::string & sqlstr )
{
    Query q; 

    q << sqlstr;

    return this->submitQuery(q);
}


bool
MysqlSession::submitQuery ( Query & q )
{
    int result = 0;

    std::string  sqlstr  = q.str();

    result = mysql_real_query(_dbcon, sqlstr.c_str(), sqlstr.length());

    if ( result != 0 ) {
        _sqlerr = "MysqlSession::submitQuery(): ";
        _sqlerr.append((std::string) mysql_error(_dbcon));
        return false;
    }

    return true;
}


bool
MysqlSession::submitQuery ( Query & q, Result & results )
{
    MYSQL_RES  * res    = NULL;
    int          fldcnt = 0;
    int          qres   = 0;
    std::string  sqlstr = q.str();

    qres = mysql_real_query(_dbcon, sqlstr.c_str(), sqlstr.length());

    if ( qres != 0 ) {
        _sqlerr = "MysqlSession::submitQuery(): ";
        _sqlerr.append((std::string) mysql_error(_dbcon));
        return false;
    } else if ( mysql_field_count(_dbcon) == 0 ) {
        _sqlerr = "MysqlSession::submitQuery(): No Results";
        return false;
    }

    res = mysql_store_result(_dbcon);

    if ( res == NULL ) {
        _sqlerr = "MysqlSession::submitQuery(): Failed to obtain results: ";
        _sqlerr.append((std::string) mysql_error(_dbcon));
        return false;
    }

    MYSQL_ROW    row;
    std::string  field;

    fldcnt = mysql_num_fields(res);

    while ( (row = mysql_fetch_row(res)) ) {
        Row              rowlist;
        unsigned long *  lengths;

        lengths = mysql_fetch_lengths(res);

        for ( int i = 0; i < fldcnt; ++i ) {

            if ( row[i] )
                field = std::string(row[i], lengths[i]);
            else
                field = "NULL";

            rowlist.push_back(field);
        }

        results.push_back(rowlist);
    }

    mysql_free_result(res);

    return true;
}

uint64_t
MysqlSession::insert_id()
{
    return((uint64_t) mysql_insert_id(_dbcon));
}

long
MysqlSession::affected_rows()
{
    return((long) mysql_affected_rows(_dbcon));
}

std::string
MysqlSession::statement_info()
{
    std::string  infostr;
    const char * info   = mysql_info(_dbcon);

    if ( info != NULL )
        infostr.assign(info);

    return infostr;
}

std::string
MysqlSession::character_set_name()
{
    return((std::string) mysql_character_set_name(_dbcon));
}

uint32_t
MysqlSession::server_version()
{
    return((uint32_t) mysql_get_server_version(_dbcon));
}

uint32_t
MysqlSession::client_version()
{
    return((uint32_t) mysql_get_client_version());
}

bool
MysqlSession::setCharacterSet ( const std::string & charset_name )
{
    if ( this->isConnected() )
        return false;

    if ( mysql_options(_dbcon, MYSQL_SET_CHARSET_NAME, charset_name.c_str()) == 0 )
        return true;

    return false;
}

/* Sets connection timeout option for future sessions */
void
MysqlSession::timeout ( time_t timeout )
{
    if ( timeout > 0 )
        _timeout = timeout;
    return;
}

time_t
MysqlSession::timeout()
{
    return _timeout;
}

void
MysqlSession::compression ( bool use_compression )
{
    _compress = use_compression;
}

bool
MysqlSession::compression()
{
    return _compress;
}


void
MysqlSession::setAutoReconnect ( bool auto_reconnect )
{
    my_bool reconnect = 0;

    if ( auto_reconnect )
        reconnect = 1;
    mysql_options(_dbcon, MYSQL_OPT_RECONNECT, &reconnect);

    return;
}


const std::string&
MysqlSession::sqlErrorStr()
{
    return _sqlerr;
}

void  
MysqlSession::escapeString ( std::ostringstream & q, const std::string & str )
{
    unsigned long len = ( (str.length() * 2) +1 );
    char to[len];

    mysql_real_escape_string(_dbcon, &to[0], str.data(), str.length());
    q << to;

    return;
}

void  
MysqlSession::escapeString ( Query & q, const std::string & str )
{
    unsigned long len = ( (str.length() * 2) +1 );
    char to[len];

    mysql_real_escape_string(_dbcon, &to[0], str.data(), str.length());
    q << to;

    return;
}

std::string 
MysqlSession::escapeString ( const std::string & str )
{
    unsigned long len  = ( (str.length() * 2) +1 );
    std::string   estr;
    char          to[len];

    mysql_real_escape_string(_dbcon, &to[0], str.data(), str.length());
    estr.assign(&to[0]);

    return estr;
}


} // namespace


//  _TCASQL_MYSQLSESSION_CPP_

