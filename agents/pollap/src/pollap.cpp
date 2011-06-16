
#include <string>
#include <iostream>
#include <cstdlib>

#include "CiscoSnmpSession.h"
#include "SnmpOid.h"
using namespace tcasnmp;

#include "PollerConfig.h"
using namespace pollap;



void usage()
{
    std::cout << "Usage: 'pollap' host community" << std::endl;
    exit(0);
}


void showResult ( SnmpSession & snmp, SnmpPdu * response )
{
    std::string  foo;
    char  t;
    int   type;
    int   f   = 0;

    response->getResponseType(t);
    type = (int) t;

    switch ( t ) {
        case ASN_OCTET_STR:
            response->getString(foo);
            std::cout  << foo;
            break;
        case ASN_INTEGER:
            response->getInteger(f);
            std::cout <<  f;
            break;
        case ASN_OBJECT_ID:
            response->getObjectId(foo);
            std::cout << foo;
            break;
        default:
            response->getString(foo);
            std::cout << foo;
            break;
    }
}

int performQuery ( SnmpSession & snmp, PollerQuery & query )
{
    PollerQueryList::iterator qIter;

    SnmpPdu  * response = NULL;

    std::cout << "performQuery " << query.query_type << std::endl;

    if ( query.query_type == SNMPQUERY_TYPE_GETNEXT ) {
        std::cout << " query " << query.oid.getOidStr() << std::endl;
        response = snmp.snmpGetNext(query.oid.getOidStr().c_str());

        while ( response ) {
            std::string  respoid;

            response->getOid(respoid);

            std::cout << "response oid:" << respoid << std::endl;
            std::string keyname = respoid.substr(query.oid.getOidStr().length() + 1);
            std::string keystr  = SnmpOid::StringFromBitString(keyname);
            std::cout << " key = " << keystr; 
            std::string keyr    = SnmpOid::StringToBitString(keystr);

            std::cout << " status = ";
            showResult(snmp, response);

            std::cout << std::endl;

            response = snmp.snmpGetNext(response);
        }
    }

    return 0;
}


int main ( int argc, char **argv ) 
{
    SnmpSession  snmp;
    std::string  host, comm, config;

    if ( argc != 4 )
        usage();

    host = argv[1];
    comm = argv[2];

    if ( host.empty() )
        usage();
   
    PollerConfig     pcfg;
    PollerQueryMap   querymap;

    if ( pcfg.parseQueryConfig(argv[3], querymap) )
        std::cout << "parse was successful " << std::endl;

    std::cout << "query size is " << querymap.size() << std::endl;
 

    if ( ! snmp.openSession(host, comm) ) {
        std::cout << "failed to open session: " << snmp.errorStr() << std::endl;
        return 0;
    }


    PollerQueryMap::iterator   sIter;
    PollerQueryList::iterator  qIter;

    for ( sIter = querymap.begin(); sIter != querymap.end(); ++sIter ) 
    {
        std::cout << " Running queries for query section " << sIter->second.name 
                  << std::endl;

        for ( qIter = sIter->second.subQueries.begin(); qIter != sIter->second.subQueries.end(); ++qIter )
            performQuery(snmp, *qIter);

    }

    
    return 0;

}




