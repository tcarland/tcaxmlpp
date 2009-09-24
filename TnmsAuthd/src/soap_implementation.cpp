#define _SOAP_IMPLEMENTATION_CPP_

#include <string>

#include "soapH.h"

#include "CidrUtils.h"
using namespace tcanetpp;

#include "AuthDbThread.h"
using namespace tnmsauth;



//--------------------------------------------------------------------//
// authenticate

int ns1__authenticate ( soap            * s,
                        xsd__string       username, 
		        xsd__string       password,
		        xsd__string       ipaddress,
		        ns1__AuthResult & result )
{
    std::string  ticket, user, pass, ipaddr;
    size_t       ticket_len;
    time_t       now  = ::time(NULL);

    AuthDbThread * authth = (AuthDbThread*) s->user;

    if ( username )
        user = username;
    if ( password )
        pass = password;
    if ( ipaddress )
        ipaddr = ipaddress;
    
    result.success = authth->authenticate(user, pass, ipaddr, now, ticket);
    ticket_len     = ticket.length();

    result.ticket = (char*) soap_malloc(s, ticket_len + 1);
    result.ticket[ticket_len] = '\0';

    strncpy(result.ticket, ticket.c_str(), ticket_len);

    result.timeout = TICKET_REFRESH_INTERVAL;
    result.message = "tnmsauth";

    return SOAP_OK;
}


int ns1__refresh ( soap         * s,
                   xsd__string    username,
                   xsd__string    ticket,
                   xsd__string    ipaddress,
                   xsd__boolean & result )
{
    std::string  user, tk, ipaddr;
    time_t       now  = ::time(NULL);
    
    AuthDbThread * authth = (AuthDbThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tk = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    result = authth->refreshTicket(user, tk, ipaddr, now);

    return SOAP_OK;
}


int ns1__expire ( soap         * s,
                  xsd__string    username,
                  xsd__string    ticket,
                  xsd__string    ipaddress,
                  xsd__boolean & result )
{
    std::string  user, tick, ipaddr;
    
    AuthDbThread * authth = (AuthDbThread*) s->user;

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
    std::string  user, tk, ipaddr;
    
    AuthDbThread * authth = (AuthDbThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tk = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    result = authth->isAuthentic(user, tk, ipaddr);

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
    std::string   user, tk, ipaddr, resource;
    
    AuthDbThread * authth = (AuthDbThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tk = ticket;
    if ( resourcename )
        resource = resourcename;
    if ( ipaddress )
        ipaddr = ipaddress;

    result = authth->isAuthentic(user, tk, ipaddr);

    if ( result ) 
        result = authth->isAuthorized(user, tk, ipaddr, resource);

    return SOAP_OK;
}


int ns1__getAuthorizations ( soap        * s,
			     xsd__string   username,
			     xsd__string   ticket,
			     xsd__string   ipaddress,
			     struct ns1__getAuthorizationsResponse & result)
{
    std::string  user, tk, ipaddr;

    AuthDbThread * authth = (AuthDbThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tk = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    if ( ! authth->isAuthentic(user, tk, ipaddr) )
        return SOAP_OK;

    StringList   authlist;

    if ( ! authth->authorizations(user, tk, ipaddr, authlist) )
        return SOAP_OK;

    StringList::iterator  sIter;

    result.result.size = authlist.size();
    result.result.ptr  = (char**) soap_malloc(s, (sizeof(char*) * authlist.size()));
 
    // this makes my head hurt
    int  i = 0;
    for ( sIter = authlist.begin(); sIter != authlist.end(); ++sIter, ++i ) {
        result.result.ptr[i] = (char*) soap_malloc(s, sIter->length()+ 1);
        strncpy(result.result.ptr[i], sIter->c_str(), sIter->length());
        result.result.ptr[i][sIter->length()] = '\0';
    }
    
    return SOAP_OK;
}



//--------------------------------------------------------------------//
//  Collectors


int ns1__getCollectorList ( soap * s,
                            xsd__string username,
                            xsd__string ticket,
                            xsd__string ipaddress,
                            struct ns1__getCollectorListResponse & result )
{    
    std::string  user, tk, ipaddr;

    AuthDbThread * authth = (AuthDbThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tk = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    if ( ! authth->isAuthentic(user, tk, ipaddr) )
        return SOAP_OK;

    StringList   reflist;

    if ( ! authth->getCollectors(user, tk, ipaddr, reflist) )
        return SOAP_OK;

    StringList::iterator  sIter;

    result.result.size = reflist.size();
    result.result.ptr  = (char**) soap_malloc(s, (sizeof(char*) * reflist.size()));

    int i = 0;
    for ( sIter = reflist.begin(); sIter != reflist.end(); ++sIter, ++i ) {
        result.result.ptr[i] = (char*) soap_malloc(s, sIter->length() + 1);
        strncpy(result.result.ptr[i], sIter->c_str(), sIter->length());
        result.result.ptr[i][sIter->length()] = '\0';
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
    std::string  user, tk, ipaddr;

    AuthDbThread * authth = (AuthDbThread*) s->user;

    if ( username )
        user = username;
    if ( ticket )
        tk = ticket;
    if ( ipaddress )
        ipaddr = ipaddress;

    if ( ! authth->isAuthentic(user, tk, ipaddr) )
        return SOAP_OK;

    StringList   reflist;

    if ( ! authth->getAuthTypes(reflist) )
        return SOAP_OK;

    StringList::iterator  sIter;

    result.result.size = reflist.size();
    result.result.ptr  = (char**) soap_malloc(s, (sizeof(char*) * reflist.size()));

    int i = 0;
    for ( sIter = reflist.begin(); sIter != reflist.end(); ++sIter, ++i ) {
        result.result.ptr[i] = (char*) soap_malloc(s, sIter->length() + 1);
        strncpy(result.result.ptr[i], sIter->c_str(), sIter->length());
        result.result.ptr[i][sIter->length()] = '\0';
    }

    return SOAP_OK;
}



