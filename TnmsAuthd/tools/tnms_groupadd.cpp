#define _TNMSAUTHD_GROUPADD_CPP_

#include <string.h>

#include <iostream>
#include <iomanip>
#include <string>

#include "SqlSession.hpp"
using namespace tcqsqlpp;

#include "StringUtils.h"
using namespace tcanetpp;


static const
char TNMSAUTH_GROUPADD_VERSION [] = "v0.11";


const char * process  = "tnms_groupadd";

const char * _Dbhost  = "localhost";
const char * _Dbuser  = "tnmsauth";
const char * _Dbname  = "tnmsauthd";
const char * _Dbpass  = "tnmsauth11b";


void
usage()
{
    std::cout << "Usage: %s [-DhiLV] -g groupname -G groupdesc " << std::endl
              << "      -h         : Display help and exit" << std::endl
              << "      -D         : Delete the existing target" << std::endl
              << "      -g <name>  : The group name being added" << std::endl
              << "      -G <desc>  : An optional description of the group" << std::endl
              << "      -i         : Mark group as an internal (reserved) group name" << std::endl
              << "      -L         : List all groups" << std::endl
              << "      -S <ip>    : Default server for group members" << std::endl
              << "      -X <ip>    : Adds an access filter to the group restricting" << std::endl
              << "                    members access only from the provided IP addr (range)" << std::endl
              << "                    eg. -X '10.10.1.0/24'" << std::endl
              << "      -Y <name>  : Adds a group authorization for the provided subtree name" << std::endl
              << "                    The authorization filter should be set as either an Include" << std::endl
              << "                    or Exclude by appending ':i' or nothing respectively" << std::endl
              << "      -V         : Display version info and exit" << std::endl;
}

void
version
{
    std::cout << process << " version: " << TNMSAUTH_GROUPADD_VERSION << std::endl;
}


bool
listGroups ( SqlSession * sql, uint32_t gid )
{
    Query        query = sql->newQuery();
    Result       res;
    Row          row;

    Result::iterator  rIter;

    query << "SELECT g.gid, g.name, g.description, g.internal "
          << "FROM " << _Dbname;

    if ( gid > 0 )
        query << "WHERE g.gid=" << gid;

    query << " ORDER BY g.name";

    
    if ( ! sql->submitQuery(query, res) ) {
        std::cout << "Error listing groups: " << sql->sqlErrorStr() << std::endl;
        return false;
    }

    if ( res.size() == 0 ) {
        std::cout << "no results" << std::endl;
        return false;
    }

    std::cout << std::endl;
    std::cout << std::setiosflags(std::ios::left)
        << std::setw(20) << "  group name "
        << std::setw(8)  << "  gid "
        << std::setw(32) << "  description "
        << std::setw(5)  << " internal"
        << std::endl;

    std::cout << std::setiosflags(std::ios::left)
        << std::setw(20) << "---------------"
        << std::setw(8)  << "-----"
        << std::setw(32) << "-------------------"
        << std::setw(5)  << "-------"
        << std::resetiosflags(std::ios::left) << std::endl;

    for ( rIter = res.begin(); rIter != res.end(); ++rIter )
    {
        row = (Row) *rIter;

        std::cout << std::setiosflags(std::ios::left)
            << std::setw(20) << ( (std::string) row[1] )
            << std::setw(8)  << ( (std::string) row[0] )
            << std::setw(32) << ( (std::string) row[2] )
            << std::setw(5)  << ( (std::string) row[3] )
            << std::endl;
    }
    std::cout << std::resetiosflags(std::ios::left) << std::endl << std::endl;

    return true;
}


int main ( int argc, char **argv )
{
    char      optChar;
    char    * groupc    = NULL;
    char    * descc     = NULL;
    bool      delgroup  = false;
    bool      listgroup = false;
    bool      showint   = false;

    std::string  group, desc;

    while ( (optChar = getopt(argc, argv, "Dg:hiLV")) != EOF ) {
        switch ( optChar ) {
            case 'D':
                delgroup = true;
                break;
            case 'g':
                groupc = strdup(optarg);
                break;
            case 'L':
                listgroup = true;
                break;
            case 'V':
                version();
                break;
            default:
                usage();
                break;
        }
    }

    if ( groupc ) {
        group = groupc;
        ::free(groupc);
    }

    if ( descc ) {
        pw = pwc;
        ::free(pwc);
    }

    if ( group.empty() && ! listgroup )
        usage();

    SqlSession * sql  = NULL;
    uint32_t     gid  = 0;

    try {
        sql = new SqlSession(_Dbname, _Dbhost, _Dbuser, _Dbpass);
    } catch ( SqlException & err ) {
        std::cout << "Fatal Error initializing connection to database: "
            << err.toString() << std::endl;
        exit(1);
    }

    if ( ! sql->dbconnect() ) {
        std::cout << "Fatal error connecting to database: "
            << sql->sqlErrorStr() << std::endl;
        delete sql;
        exit(1);
    }

    if ( ! group.empty() )
        gid = getGID(sql, group);

    if ( listgroup ) {
        listGroups(sql, gid);
        delete sql;
        exit(0);
    }

    if ( gid > 0 ) 
    {
        if ( delgroup) {
            ;
            //deleteGroup(sql, group, gid);
        } else {
            if ( ! updateGroup(sql, gid, name, desc) )
                gid = 0;
        }
    }
    else 
    {
        if ( delgroup ) {
            std::cout << "Group '" << group << "' doesn't exist" << std::endl;
        } else {
            gid = addGroup(sql, user, pw, gid, auid);
        }
    }


    sql->dbclose();
    delete sql;

    return 0;

}

