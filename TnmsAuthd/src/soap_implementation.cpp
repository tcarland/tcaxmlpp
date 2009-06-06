#define _SOAP_IMPLEMENTATION_CPP_


#include <string>


#include "soapH.h"

#include "CidrUtils.h"
using namespace tcanetpp;

#include "TnmsAuthThread.h"
using namespace tnmsauth;


//ticketmgr



int ns1__authenticate ( soap         * s,
                        xsd__string    username, 
		        xsd__string    password,
		        xsd__string    ipaddress,
		        ns1__AuthenticationResult & result )
{
    std::string  ticket, user, pass, ipaddr;
    size_t       ticket_len;

    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( password )
        pass = password;
    if ( ipaddress )
        ipaddr = ipaddress;
    
    result.success = authth->authenticate(user, pass, ipaddr, ticket);
    ticket_len     = ticket.length();

    result.ticket = (char*) soap_malloc(s, ticket_len + 1);
    result.ticket[ticket_len] = '\0';

    strncpy(result.ticket, ticket.c_str(), ticket_len);

    result.timeout = TICKETD_TICKET_REFRESH_INTERVAL;
    result.message = "soemonauthd";

    return SOAP_OK;
}

int ns1__refreshTicket ( soap         * s,
                         xsd__string    username,
                         xsd__string    ticket,
                         xsd__string    ipaddress,
                         xsd__boolean & result )
{
    std::string  user, tick, ipaddr;
    
    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    result = authth->refreshTicket(user, tick, ipaddr);

    return SOAP_OK;
}

int ns1__expireTicket ( soap         * s,
                        xsd__string    username,
		        xsd__string    ticket,
		        xsd__string    ipaddress,
		        xsd__boolean & result )
{
    std::string  user, tick, ipaddr;
    
    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    result = authth->expireTicket(user, tick, ipaddr);

    return SOAP_OK;
}


int ns1__isAuthentic ( soap         * s,
		       xsd__string    username,
		       xsd__string    ticket,
		       xsd__string    ipaddress,
		       xsd__boolean & result)
{
    std::string  user, tick, ipaddr;
    
    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    result = authth->isAuthentic(user, tick, ipaddr);

    return SOAP_OK;
}

//--------------------------------------------------------------------//
//  Authorization


int ns1__authorized ( soap         * s,
		      xsd__string    username,
		      xsd__string    ticket,
		      xsd__string    ipaddress,
		      xsd__string    resourcename,
		      xsd__boolean & result)
{
    std::string  user, tick, ipaddr, resource;
    
    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( resourcename )
        resource = resourcename;
    if ( ipaddress )
        ipaddr = ipaddress;

    result = authth->isAuthentic(user, tick, ipaddr);

    if ( result ) 
        result = authth->isAuthorized(user, resource);

    return SOAP_OK;
}


int ns1__getAuthorizations ( soap        * s,
			     xsd__string   username,
			     xsd__string   ticket,
			     xsd__string   ipaddress,
			     struct ns1__getAuthorizationsResponse & result)
{
    std::string  user, tick, ipaddr;

    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    if ( ! authth->isAuthentic(user, tick, ipaddr) )
        return SOAP_OK;

    StringList   authlist;

    if ( ! authth->authorizations(user, authlist) )
        return SOAP_OK;

    StringList::iterator  sIter;

    result._return.__size = authlist.size();
    result._return.__ptr  = (char**) soap_malloc(s, (sizeof(char*) * authlist.size()));
 
    // this makes my head hurt
    int  i = 0;
    for ( sIter = authlist.begin(); sIter != authlist.end(); ++sIter, ++i ) {
        result._return.__ptr[i] = (char*) soap_malloc(s, sIter->length()+ 1);
        strncpy(result._return.__ptr[i], sIter->c_str(), sIter->length());
        result._return.__ptr[i][sIter->length()] = '\0';
    }
    
    return SOAP_OK;
}


//--------------------------------------------------------------------//
//  Client Permissions

int ns1__getClientPermissionsList ( soap * s,
                                    xsd__string username,
                                    xsd__string ticket,
                                    xsd__string ipaddress,
                                    struct ns1__getClientPermissionsResponse & result )
{
    std::string  user, tick, ipaddr;

    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    if ( ! authth->isAuthentic(user, tick, ipaddr) )
        return SOAP_OK;

    PermissionList bitfields;
    if ( ! authth->getPermissions(user, bitfields) )
        return SOAP_OK;
    
    PermissionList::iterator  bIter;
    result._return.__size = bitfields.size();
    result._return.__ptr  = (unsigned int*) soap_malloc(s, (sizeof(unsigned int*) * bitfields.size()));

    int  i  = 0;
    unsigned int bitfield = 0;

    for ( bIter = bitfields.begin(); bIter != bitfields.end(); ++bIter, ++i ) {
        bitfield  = *bIter;
        result._return.__ptr[i] = bitfield;
    }

    return SOAP_OK;
}


//--------------------------------------------------------------------//
//  Reflectors


int ns1__getCollectorList ( soap * s,
                            xsd__string username,
                            xsd__string ticket,
                            xsd__string ipaddress,
                            struct ns1__getReflectorListResponse & result )
{    
    std::string  user, tick, ipaddr;

    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    if ( ! authth->isAuthentic(user, tick, ipaddr) )
        return SOAP_OK;

    StringList   reflist;

    if ( ! authth->getReflectors(user, reflist) )
        return SOAP_OK;

    StringList::iterator  sIter;

    result._return.__size = reflist.size();
    result._return.__ptr  = (char**) soap_malloc(s, (sizeof(char*) * reflist.size()));

    int i = 0;
    for ( sIter = reflist.begin(); sIter != reflist.end(); ++sIter, ++i ) {
        result._return.__ptr[i] = (char*) soap_malloc(s, sIter->length() + 1);
        strncpy(result._return.__ptr[i], sIter->c_str(), sIter->length());
        result._return.__ptr[i][sIter->length()] = '\0';
    }

    return SOAP_OK;
}


//--------------------------------------------------------------------//
//  AuthTypes


int ns1__getAuthTypesList ( soap * s,
                            xsd__string username,
                            xsd__string ticket,
                            xsd__string ipaddress,
                            struct ns1__getAuthTypesListResponse & result )
{    
    std::string  user, tick, ipaddr;

    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    if ( ! authth->isAuthentic(user, tick, ipaddr) )
        return SOAP_OK;

    StringList   reflist;

    if ( ! authth->getAuthTypes(reflist) )
        return SOAP_OK;

    StringList::iterator  sIter;

    result._return.__size = reflist.size();
    result._return.__ptr  = (char**) soap_malloc(s, (sizeof(char*) * reflist.size()));

    int i = 0;
    for ( sIter = reflist.begin(); sIter != reflist.end(); ++sIter, ++i ) {
        result._return.__ptr[i] = (char*) soap_malloc(s, sIter->length() + 1);
        strncpy(result._return.__ptr[i], sIter->c_str(), sIter->length());
        result._return.__ptr[i][sIter->length()] = '\0';
    }

    return SOAP_OK;
}


//--------------------------------------------------------------------//
//  Preferences


int ns1__getPackages ( soap * s,
                       xsd__string username,
                       xsd__string ticket,
                       xsd__string ipaddress,
                       ns1__getPackageListResponse & result )
{
    std::string  user, tick, ipaddr;

    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    if ( ! authth->isAuthentic(user, tick, ipaddr) )
        return SOAP_OK;

    result.entries = soap_new_std__vectorTemplateOfns1__PackageInfo(s, 1);
    result.entries->resize(0);

    PackageList  pkglist;
    PackageList::iterator  pIter;

    if ( ! authth->getPackages(user, pkglist) )
        return SOAP_OK;

    for ( pIter = pkglist.begin(); pIter != pkglist.end(); ++pIter ) {
        ns1__PackageInfo  pkginfo;

        pkginfo.owner = (char*) soap_malloc(s, pIter->pkg_owner.length() + 1);
        strncpy(pkginfo.owner, pIter->pkg_owner.c_str(), pIter->pkg_owner.length());
        pkginfo.owner[pIter->pkg_owner.length()] = '\0';

        pkginfo.name = (char*) soap_malloc(s, pIter->pkg_name.length() + 1);
        strncpy(pkginfo.name, pIter->pkg_name.c_str(), pIter->pkg_name.length());
        pkginfo.name[pIter->pkg_name.length()] = '\0';

        pkginfo.desc = (char*) soap_malloc(s, pIter->pkg_desc.length() + 1);
        strncpy(pkginfo.desc, pIter->pkg_desc.c_str(), pIter->pkg_desc.length());
        pkginfo.desc[pIter->pkg_desc.length()] = '\0';

        pkginfo.version = (char*) soap_malloc(s, pIter->version.length() + 1);
        strncpy(pkginfo.version, pIter->version.c_str(), pIter->version.length());
        pkginfo.version[pIter->version.length()] = '\0';

        pkginfo.timestamp = pIter->timestamp;
        pkginfo.is_public = pIter->is_public;

        result.entries->push_back(pkginfo);
    }

    return SOAP_OK;
}


int ns1__getPackage ( soap * s,
                      xsd__string username,
                      xsd__string ticket,
                      xsd__string ipaddress,
                      xsd__string package_owner,
                      xsd__string package_name,
                      struct ns1__getPackageResponse & result )
{    
    std::string  user, tick, ipaddr;
    std::string  owner, name;

    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;
    if ( package_owner )
        owner = package_owner;
    if ( package_name )
        name = package_name;

    if ( ! authth->isAuthentic(user, tick, ipaddr) )
        return SOAP_OK;

    std::string  package;

    if ( ! authth->getPackage(user, owner, name, package) || package.empty() )
        return SOAP_OK;

    result._return.pkg  = (char*) soap_malloc(s, package.length() + 1);
    strncpy(result._return.pkg, package.c_str(), package.length());
    result._return.pkg[package.length()] = '\0';

    return SOAP_OK;
}


int ns1__submitPackage ( soap * s,
                         xsd__string    username,
                         xsd__string    ticket,
                         xsd__string    ipaddress,
                         xsd__string    package_name,
                         xsd__string    package_desc,
                         xsd__string    package_vers,
                         xsd__dateTime  package_time,
                         xsd__string    package,
                         xsd__boolean   is_public,
                         xsd__boolean & result )
{
    std::string  user, tick, ipaddr;
    std::string  name, desc, pkg;

    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;
    if ( package_name )
        name = package_name;
    if ( package_desc )
        desc = package_desc;
    if ( package )
        pkg = package;

    result = false;

    if ( ! authth->isAuthentic(user, tick, ipaddr) )
        return SOAP_OK;

    if ( authth->submitPackage(user, name, desc, package_vers, package_time, pkg, is_public) )
        result = true;

    return SOAP_OK;
}


int  ns1__removePackage ( soap * s,
                          xsd__string  username,
                          xsd__string  ticket,
                          xsd__string  ipaddress,
                          xsd__string  package_name,
                          xsd__boolean & result )
{
    std::string  user, tick, ipaddr, pkgname;
    
    TnmsAuthThread * authth = (TnmsAuthThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tick = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;
    if ( package_name )
        pkgname = package_name;

    result = false;

    if ( authth == NULL )
        return SOAP_OK;

    if ( ! authth->isAuthentic(user, tick, ipaddr) )
        return SOAP_OK;

    result = authth->removePackage(user, pkgname);
    
    return SOAP_OK;
}



