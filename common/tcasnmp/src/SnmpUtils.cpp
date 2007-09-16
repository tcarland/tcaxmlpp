/**  SnmpUtils
  *
  *
 **/
#define _TCASNMP_SNMPUTILS_CPP_

#include "SnmpUtils.h"

#include "StringUtils.h"
#include "CidrUtils.h"
using namespace tcanetpp;


namespace tcasnmp {


void
SnmpUtils::MapInterfaces ( SnmpSession * session, std::map<int, std::string> & ifmap )
{
    SnmpPdu *    response;
    std::string  desc;
    int          indx;
    
    std::map<int, std::string>  ifMap;

    response = session->snmpGetNext(IFINDEX_OID);

    while ( response ) {

	if ( ! response->getInteger(indx) )
	    break;

	desc = SnmpUtils::QueryIfDescr(session, indx);
	ifmap[indx] = desc;

	response = session->snmpGetNext(response);
    }

    if ( response )
        SnmpSession::ReleasePDU(response);

    return;
}


void
SnmpUtils::MapInterfaces ( SnmpSession * session, NetworkDevice & device )
{
    std::map<int, std::string>      ifmap;
    std::map<int, ipv4addr_t>       ipmap;
    IfList                          ifv;

    std::map<int, std::string>::iterator  fIter;
    std::map<int, ipv4addr_t>::iterator   pIter;

    SnmpUtils::MapInterfaces(session, ifmap);
    SnmpUtils::QueryIpTable(session, ipmap);

    for ( fIter = ifmap.begin(); fIter != ifmap.end(); fIter++ ) {
	int         indx  = fIter->first;
	std::string alias = "";

	NetworkInterface iface(device.deviceAddr(), indx);
	iface.ifName(fIter->second);

	if ( (pIter = ipmap.find(indx)) != ipmap.end() )
	    iface.ifAddr(ipmap[indx]);

	// get description
	alias = SnmpUtils::QueryIfAlias(session, indx);
	if ( alias != "" )
	    iface.description(alias);

	ifv.push_back(iface);
    }
    device.setNetworkInterfaces(ifv);

    return;
}


void
SnmpUtils::QueryIpTable ( SnmpSession * session, std::map<int, ipv4addr_t> & ipmap )
{
    SnmpPdu *               response;
    std::vector<ipv4addr_t> ipv;
    ipv4addr_t              addr;
    std::string             errstr;

    std::string toid = IP_ADDRENTRY_OID;

    response = session->snmpGetNext(toid.c_str());

    while ( response ) {

	if ( response->pdu->variables->type == ASN_APPLICATION ) {
            response->getUnsignedInteger(addr);
	    ipv.push_back(addr);
	} else {
            errstr = "Invalid type: ";
            errstr.append(StringUtils::toString(response->pdu->variables->type));
            session->errorStr(errstr);
	    break;
	}

	response = session->snmpGetNext(response);
    }

    if ( response )
        SnmpSession::ReleasePDU(response);

    // Get ifindex for associated IP's
    std::vector<ipv4addr_t>::iterator vIter;
    
    for ( vIter = ipv.begin(); vIter != ipv.end(); vIter++ ) {
	int  indx = 0;
	
        addr = (ipv4addr_t) *vIter;
	toid = IP_INDXENTRY_OID;
	toid.append(".").append(Cidr::ntop(addr));

	response = session->snmpGet(toid.c_str());

	if ( response == NULL )
	    continue;

	response->getInteger(indx);
	ipmap[indx] = addr;

	if ( response )
	    SnmpSession::ReleasePDU(response);
    }

    return;
}


std::string
SnmpUtils::QueryIfDescr ( SnmpSession * session, long indx )
{
    SnmpPdu     * response;
    std::string   toid, ifDescr;

    toid     = IFDESCR_OID;
    toid.append(".");
    toid.append(StringUtils::toString(indx));

    response = session->snmpGet(toid.c_str());

    if ( response == NULL )
	return ifDescr;

    response->getString(ifDescr);

    if ( response )
	SnmpSession::ReleasePDU(response);


    return ifDescr;
}


std::string
SnmpUtils::QueryIfAlias ( SnmpSession * session, long indx )
{
    SnmpPdu     * response;
    std::string   toid, ifAlias;

    toid     = IFALIAS_OID;
    toid.append(".");
    toid.append(StringUtils::toString(indx));

    response = session->snmpGet(toid.c_str());

    if ( ! response )
	return ifAlias;

    response->getString(ifAlias);

    if ( response )
	SnmpSession::ReleasePDU(response);

    return ifAlias;
}


int
SnmpUtils::GetCDPNeighbors ( SnmpSession * session, DeviceSet & devlist )
{
    SnmpPdu    * response;
    std::string  rsp;
    int          total  = 0;

    response = session->snmpGetNext(CDP_DEVICEID_OID);

    while ( response ) {
        NetworkDevice device;

        response->getString(rsp);

        if ( !rsp.empty() && device.setDevice(rsp) ) {
            device.community(session->community());
            
            devlist.insert(device);
            total++;
        }

	response = session->snmpGetNext(response);
    }

    if ( response )
	SnmpSession::ReleasePDU(response);

    return total;
}


}  // namespace

/*  _TCASNMP_SNMPUTILS_CPP_  */
