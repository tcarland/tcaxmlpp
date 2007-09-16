/** SnmpSession
  * 
  *  @Author   tcarland@gmail.com
  *  @Version  3.1
 **/
#define _TCASNMP_SNMPSESSION_CPP_

#include <stdio.h>

#include "SnmpSession.h"
#include "oids.h"


#include "StringUtils.h"
using namespace tcanetpp;


namespace tcasnmp {


bool
SnmpSession::_NetSnmpLibInit = false;



SnmpSession::SnmpSession()
    : _sptr(NULL),
      _baseOid(NULL)

{
    this->init();
}


SnmpSession::SnmpSession ( const std::string & host, const std::string & community )
    : _sptr(NULL),
      _baseOid(NULL)
{
    this->init();
    this->openSession(host, community);
}


SnmpSession::SnmpSession ( const NetworkDevice & device )
    : _sptr(NULL),
      _baseOid(NULL)
{
    this->init();
    this->openSession(device, false);
}


SnmpSession::~SnmpSession()
{
    if ( _baseOid )
        SNMP_FREE(_baseOid);
    this->closeSession();
}


void
SnmpSession::init()
{
    SnmpSession::LibInit();
    snmp_sess_init(&_session);
}


bool
SnmpSession::openSession ( const std::string & host, const std::string & community )
{
    char  *errstr;
    int    liberr, syserr;

    this->closeSession();

    _community             = community;
    _session.peername      = (char*) host.c_str();
    _session.version       = SNMP_VERSION_1;
    _session.community     = (u_char*) community.c_str();
    _session.community_len = community.length();

    _sptr = snmp_sess_open(&_session);

    if ( _sptr == NULL ) {
	snmp_error(&_session, &liberr, &syserr, &errstr);
	_errStr = "SnmpSession::openSession() ";
	_errStr.append(errstr);
	::free(errstr);
	return false;
    }

    return true;
}


bool
SnmpSession::openSession ( const NetworkDevice & device, bool write )
{
    std::string comm = "";

    if ( write )
        comm = device.writeCommunity();
    else
        comm = device.readCommunity();

    return this->openSession(device.deviceName(), comm);
}


void
SnmpSession::closeSession()
{
    _community = "";
    if ( _sptr != NULL )
	snmp_sess_close(_sptr);
    _sptr = NULL;
    return;
}


SnmpPdu*
SnmpSession::get ( const std::string & toid )
{
    size_t   oid_len = MAX_OID_LEN;
    oid      anOID[oid_len];
    char*    errstr;
    int      status, liberr, syserr;

    struct snmp_pdu  *pdu, *response;

    if ( _sptr == NULL )
	return NULL;

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    snmp_parse_oid(toid.c_str(), anOID, &oid_len);
    snmp_add_null_var(pdu, anOID, oid_len);

    status = snmp_sess_synch_response(_sptr, pdu, &response);

    if ( status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR ) {
	_errStr = "";
	return(new SnmpPdu(response));
    } else {
	snmp_sess_error(_sptr, &liberr, &syserr, &errstr);
	_errStr = "SnmpSession::get(): ";
	_errStr.append(errstr);
	::free(errstr);
    }

    return NULL;
}


SnmpPdu*
SnmpSession::getNext ( const std::string & toid )
{
    size_t   oid_len = MAX_OID_LEN;
    oid      anOID[oid_len];

    struct snmp_pdu  *pdu;

    if ( _sptr == NULL )
	return NULL;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    snmp_parse_oid(toid.c_str(), anOID, &oid_len);
    snmp_add_null_var(pdu, anOID, oid_len);

    if ( _baseOid )
        SNMP_FREE(_baseOid);
    
    _baseOid = snmp_duplicate_objid(anOID, oid_len);
    _baseLen = oid_len;

    return this->getNext(new SnmpPdu(pdu));
}

SnmpPdu*
SnmpSession::getNext ( SnmpOid & oid )
{
    return NULL;
}
    
SnmpPdu*
SnmpSession::getNext ( SnmpPdu * spdu )
{
    char* errstr;
    int   status, liberr, syserr;

    if ( spdu == NULL )
	return NULL;

    struct snmp_pdu * response;
    
    response          = spdu->pdu;
    response->command = SNMP_MSG_GETNEXT;

    status = snmp_sess_synch_response(_sptr, response, &response);

    if ( status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR ) {
	_errStr = "";

        if ( ! this->hasBaseOid(response) ) {
            SNMP_FREE(_baseOid);
            _baseOid = NULL;
            _baseLen = 0;
        }
        
        spdu->pdu = response;

	return spdu;
    } else {
	snmp_sess_error(_sptr, &liberr, &syserr, &errstr);
	_errStr = "SnmpSession::getNext(): ";
	_errStr.append(errstr);
	::free(errstr);
    }
    
    return NULL;
}


bool
SnmpSession::set ( const std::string & toid, char type, const std::string & value )
{
    size_t   oid_len = MAX_OID_LEN;
    oid      anOID[oid_len];
    char*    errstr;
    int      status, liberr, syserr;
    bool     result = false;

    struct snmp_pdu  *pdu, *response;
    
    if ( _sptr == NULL )
	return result;

    pdu = snmp_pdu_create(SNMP_MSG_SET);

    snmp_parse_oid(toid.c_str(), anOID, &oid_len);
    snmp_add_var(pdu, anOID, oid_len, type, value.c_str());

    status = snmp_sess_synch_response(_sptr, pdu, &response);

    if ( status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR ) {
	_errStr = "";
	result  = true;
    } else {
	snmp_sess_error(_sptr, &liberr, &syserr, &errstr);
	_errStr = "SnmpSession::set(): ";
	_errStr.append(errstr);
	::free(errstr);
    }

    if ( response )
	snmp_free_pdu(response);

    return result;
}


bool
SnmpSession::hasBaseOid ( struct snmp_pdu * response )
{
    if ( netsnmp_oid_is_subtree(_baseOid, _baseLen,
                response->variables->name, response->variables->name_length) == 0 )
        return true;
    
    return false;
}


void
SnmpSession::ReleasePDU ( SnmpPdu * spdu )
{
    if ( spdu == NULL )
        return;

    delete spdu;

    return;
}


std::string
SnmpSession::getSysDescr()
{
    std::string  toid  = SYSDESCR_OID;
    std::string  desc  = "";

    SnmpPdu  * response = NULL;

    response = this->get(toid.c_str());

    if ( response ) {
        response->getString(desc);
        SnmpSession::ReleasePDU(response);
    }
    
    return desc;
}


std::string
SnmpSession::getSysName()
{
    std::string  toid = SYSNAME_OID;
    std::string  name = "";

    SnmpPdu  * response = NULL;

    response = this->get(toid.c_str());

    if ( response ) {
        response->getString(name);
        SnmpSession::ReleasePDU(response);
    }

    return name;
}


time_t
SnmpSession::getSysUpTime()
{
    time_t       uptime = 0;
    std::string  toid   = SYSUPTIME_OID;
    
    SnmpPdu  * response = NULL;

    response = this->get(toid.c_str());

    if ( response ) {
        response->getTimestamp(uptime);
        SnmpSession::ReleasePDU(response);
    }

    return uptime;
}


std::string
SnmpSession::community()
{
    return _community;
}


const std::string&
SnmpSession::getErrorStr()
{
    return _errStr;
}

/*
void
SnmpSession::errorStr ( const std::string & errstr )
{
    _errStr = errstr;
}
*/

void
SnmpSession::LibInit()
{
    if ( ! SnmpSession::_NetSnmpLibInit ) {
        ::init_snmp("snmpapp");
        SnmpSession::_NetSnmpLibInit = true;
    }

    return;
}


} // namespace


/*  _TCASNMP_SNMPSESSION_CPP_  */
