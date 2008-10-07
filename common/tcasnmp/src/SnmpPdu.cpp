/**  SnmpPdu
 *
 **/
#define _TCASNMP_SNMPPDU_CPP_

#include "SnmpPdu.h"


namespace tcasnmp {


SnmpPdu::SnmpPdu()
    : pdu(NULL)
{}

SnmpPdu::SnmpPdu ( struct snmp_pdu * pdu_ )
    : pdu(pdu_)
{}


SnmpPdu::~SnmpPdu()
{
    if ( pdu )
        snmp_free_pdu(pdu);
}


bool
SnmpPdu::getOid ( std::string & soid )
{
    if ( this->pdu == NULL  )
        return false;

    char  coid[MAX_OID_LEN];

    snprint_objid(coid, MAX_OID_LEN, this->pdu->variables->name, 
                  this->pdu->variables->name_length);
    soid = std::string(coid);

    return true;
}


bool
SnmpPdu::getResponseType ( char & type )
{
    if ( this->pdu == NULL )
        return false;

    type = this->pdu->variables->type;

    return true;
}


bool
SnmpPdu::getString ( std::string & val )
{
    if ( this->pdu == NULL )
        return false;

    char  * rsp = NULL;

    struct variable_list * vars = this->pdu->variables;

    if ( vars->type == ASN_OCTET_STR ) {
	rsp = (char*) malloc(1 + vars->val_len);
	memcpy(rsp, vars->val.string, vars->val_len);
	rsp[vars->val_len] = '\0';
	val = rsp;
	free(rsp);
    } else if ( vars->type == ASN_IPADDRESS ) {
        rsp = (char*) malloc(32 + 1);
        u_char  * ip = vars->val.string;
        snprintf(rsp, 32, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
        val = rsp;
        free(rsp);
    } else {
        rsp = (char*) malloc(256);
        snprint_variable(rsp, 256, vars->name, vars->name_length, vars);
        val = rsp;
        free(rsp);
    }

    return true;
}


bool
SnmpPdu::getInteger ( int & val )
{
    if ( this->pdu == NULL )
        return false;

    struct variable_list * vars = this->pdu->variables;

    if ( vars == NULL )
        return false;

    if ( vars->type == ASN_INTEGER || vars->type == ASN_TIMETICKS )
	val = *(vars->val.integer);
    else
        return false;

    return true;
}

bool
SnmpPdu::getUnsignedInteger ( uint32_t & val )
{
    if ( this->pdu == NULL )
        return false;

    struct variable_list * vars = this->pdu->variables;

    if ( vars->type == ASN_INTEGER || vars->type == ASN_TIMETICKS 
            || vars->type == ASN_APPLICATION )
        val = *(vars->val.integer);
    else
        return false;

    return true;
}


bool
SnmpPdu::getTimestamp ( time_t & t )
{
    if ( this->pdu == NULL )
        return false;

    if ( this->pdu->variables->type == ASN_TIMETICKS )
        t = *(this->pdu->variables->val.integer);
    else
        return false;

    return true;
}


bool
SnmpPdu::getObjectId ( std::string & soid )
{
    if ( this->pdu == NULL )
        return false;

    char    * rsp     = NULL;
    size_t    rsp_len = 256;

    struct variable_list * vars = this->pdu->variables;

    if ( vars->type == ASN_OBJECT_ID ) {
	rsp  = (char*) malloc(rsp_len);
	snprint_variable(rsp, rsp_len, vars->name, vars->name_length, vars);
	soid = rsp;
	free(rsp);
    } else {
        return false;
    }

    return true;
}


} // namespace

/*  _TCASNMP_SNMPPDU_CPP_ */
