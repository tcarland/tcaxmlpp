#define _SOAP_IMPLEMENTATION_CPP_

#include <string>

#include "soapH.h"

#include "AuthDbThread.h"
using namespace tnmsauth;



//--------------------------------------------------------------------//
// authenticate

int ns1__authenticate ( soap              * s,
                        xsd__string         username, 
		        xsd__string         password,
		        xsd__string         ipaddress,
		        ns1__AuthResponse & r )
{
    std::string  ticket, user, pass, ipaddr, msg;
    size_t       len;
    time_t       now  = ::time(NULL);

    AuthDbThread * authth = (AuthDbThread*) s->user;

    if ( username )
        user = username;
    if ( password )
        pass = password;
    if ( ipaddress )
        ipaddr = ipaddress;
    
    r.result.success = authth->authenticate(user, pass, ipaddr, now, ticket);

    len   = ticket.length();
    r.result.ticket = (char*) soap_malloc(s, len + 1);
    r.result.ticket[len] = '\0';

    strncpy(r.result.ticket, ticket.c_str(), len);
    
    msg = "tnmsauth";
    len = msg.length();
    r.result.message = (char*) soap_malloc(s, len + 1);
    r.result.message[len] = '\0';

    strncpy(r.result.message, msg.c_str(), len);

    r.result.timeout = TICKET_REFRESH_INTERVAL;

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
			     struct ns1__getAuthorizationsResponse & r)
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

    r.result.size = authlist.size();
    r.result.ptr  = (char**) soap_malloc(s, (sizeof(char*) * authlist.size()));
 
    // this makes my head hurt
    int  i = 0;
    for ( sIter = authlist.begin(); sIter != authlist.end(); ++sIter, ++i ) {
        r.result.ptr[i] = (char*) soap_malloc(s, sIter->length()+ 1);
        strncpy(r.result.ptr[i], sIter->c_str(), sIter->length());
        r.result.ptr[i][sIter->length()] = '\0';
    }
    
    return SOAP_OK;
}



//--------------------------------------------------------------------//
//  Collectors


int ns1__getCollectorList ( soap * s,
                            xsd__string username,
                            xsd__string ticket,
                            xsd__string ipaddress,
                            struct ns1__getCollectorListResponse & r )
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

    r.result.size = reflist.size();
    r.result.ptr  = (char**) soap_malloc(s, (sizeof(char*) * reflist.size()));

    int i = 0;
    for ( sIter = reflist.begin(); sIter != reflist.end(); ++sIter, ++i ) {
        r.result.ptr[i] = (char*) soap_malloc(s, sIter->length() + 1);
        strncpy(r.result.ptr[i], sIter->c_str(), sIter->length());
        r.result.ptr[i][sIter->length()] = '\0';
    }

    return SOAP_OK;
}


//--------------------------------------------------------------------//
//  AuthTypes


int ns1__getAuthTypesList ( soap * s,
                            xsd__string username,
                            xsd__string ticket,
                            xsd__string ipaddress,
                            struct ns1__getAuthTypesListResponse & r )
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

    r.result.size = reflist.size();
    r.result.ptr  = (char**) soap_malloc(s, (sizeof(char*) * reflist.size()));

    int i = 0;
    for ( sIter = reflist.begin(); sIter != reflist.end(); ++sIter, ++i ) {
        r.result.ptr[i] = (char*) soap_malloc(s, sIter->length() + 1);
        strncpy(r.result.ptr[i], sIter->c_str(), sIter->length());
        r.result.ptr[i][sIter->length()] = '\0';
    }

    return SOAP_OK;
}



