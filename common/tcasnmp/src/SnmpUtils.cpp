/**  SnmpUtils
  *
  *
 **/
#define _TCASNMP_SNMPUTILS_CPP_

#include "SnmpUtils.h"
#include "oids.h"


#include "StringUtils.h"
#include "IpAddr.h"
using namespace tcanetpp;


namespace tcasnmp {


void
SnmpUtils::MapInterfaces ( SnmpSession * session, IfaceNameMap & ifmap )
{
    IfaceNameMap  ifMap;
    SnmpPdu *     response;
    std::string   desc;
    int           indx;

    response = session->getNext(IFINDEX_OID);

    while ( response )
    {
	if ( ! response->getInteger(indx) )
	    break;

	desc = SnmpUtils::QueryIfDescr(session, indx);
	ifmap[indx] = desc;

	response = session->getNext(response);
    }

    if ( response )
        SnmpSession::ReleasePDU(response);

    return;
}


void
SnmpUtils::MapInterfaces ( SnmpSession * session, NetworkDevice & device )
{
    IfaceNameMap      ifmap;
    IfaceIpMap        ipmap;
    IfList            ifv;

    IfaceNameMap::iterator  fIter;
    IfaceIpMap::iterator    pIter;

    SnmpUtils::MapInterfaces(session, ifmap);
    SnmpUtils::QueryIpTable(session, ipmap);

    for ( fIter = ifmap.begin(); fIter != ifmap.end(); fIter++ ) {
	std::string alias = "";
	int         indx;
        
        indx = fIter->first;

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
SnmpUtils::QueryIpTable ( SnmpSession * session, IfaceIpMap & ipmap )
{
    SnmpPdu *               response;
    std::vector<ipv4addr_t> ipv;
    ipv4addr_t              addr;
    std::string             errstr;

    std::string toid = IP_ADDRENTRY_OID;

    response = session->getNext(toid.c_str());

    while ( response )
    {
	if ( response->pdu->variables->type == ASN_APPLICATION ) {
            response->getUnsignedInteger(addr);
	    ipv.push_back(addr);
	} else {
            errstr = "Invalid type: ";
            errstr.append(StringUtils::toString(response->pdu->variables->type));
            session->setErrorStr(errstr);
	    break;
	}

	response = session->getNext(response);
    }

    if ( response )
        SnmpSession::ReleasePDU(response);

    // Get ifindex for associated IP's
    std::vector<ipv4addr_t>::iterator vIter;
    
    for ( vIter = ipv.begin(); vIter != ipv.end(); vIter++ ) {
	int  indx = 0;
	
        addr = (ipv4addr_t) *vIter;
	toid = IP_INDXENTRY_OID;
	toid.append(".").append(IpAddr::ntop(addr));

	response = session->get(toid.c_str());

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

    response = session->get(toid.c_str());

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

    response = session->get(toid.c_str());

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

    response = session->getNext(CDP_DEVICEID_OID);

    while ( response )
    {
        NetworkDevice device;

        response->getString(rsp);

        if ( !rsp.empty() && device.setDevice(rsp) ) 
        {
            device.community(session->community());
            devlist.insert(device);
            total++;
        }

	response = session->getNext(response);
    }

    if ( response )
	SnmpSession::ReleasePDU(response);

    return total;
}


}  // namespace

//  _TCASNMP_SNMPUTILS_CPP_ 

