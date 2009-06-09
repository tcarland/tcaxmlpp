#ifndef _SOAP_DEFINITIONS_H_
#define _SOAP_DEFINITIONS_H_

#include <string.h>
#include <time.h>
#include <string>
#include <vector>


//gsoap ns1 service name: tnms
//gsoap ns1 service namespace: urn:tnms

/*  service port: http://localhost:8080  */

typedef char *   xsd__string;
typedef bool     xsd__boolean;
typedef int      xsd__int;
typedef time_t   xsd__dateTime;
typedef unsigned int xsd__unsignedInt;



//--------------------------------------------------------------------//
// authenticate


class ns1__AuthResult 
{
  public:
    xsd__string         ticket;
    xsd__string         message;
    xsd__boolean        success;
    xsd__int            timeout;
};

struct ns1__AuthResponse
{
    ns1__AuthResult     result;
};


int  ns1__authenticate  ( xsd__string       username,
                          xsd__string       password,
                          xsd__string       ipaddress,
                          ns1__AuthResult & result );

int  ns1__refresh       ( xsd__string       username,
                          xsd__string       ticket,
                          xsd__string       ipaddress,
                          xsd__boolean    & result);

int  ns1_expire         ( xsd__string       username,
                          xsd__string       ticket,
                          xsd__string       ipaddress,
                          xsd__boolean    & result );

int  ns1_isAuthentic    ( xsd__string       username,
                          xsd__string       ticket,
                          xsd__string       ipaddress,
                          xsd__boolean    & result );


//--------------------------------------------------------------------//
// authorize


struct ns1__AuthorizationList
{
    xsd__string *  ptr;
    xsd__int       size;
    xsd__boolean   isInclude;
};

struct ns1__getAuthorizationResponse
{
    ns1__AuthorizationList  result;
};


int  ns1__isAuthorized ( xsd__string    username,
                         xsd__string    ticket,
                         xsd__string    ipaddress,
                         xsd__string    resource,
                         xsd__boolean & result );

int  ns1__getAuthorizations ( xsd__string    username,
                              xsd__string    ticket,
                              xsd__string    ipaddress,
                              struct ns1__getAuthorizationResponse & result );


//--------------------------------------------------------------------//
//  Collectors


struct ns1__CollectorList
{
    xsd__string *  ptr;
    int            size;
};

struct ns1__getCollectorListResponse
{
    struct ns1__CollectorList   result;
};


int  ns1__getCollectorList  ( xsd__string  username,
                              xsd__string  ticket,
                              xsd__string  ipaddress,
                              struct ns1__getCollectorListResponse & result );


//--------------------------------------------------------------------//
//  AuthTypes

struct ns1__AuthTypesList
{
    xsd__string * ptr;
    xsd__int      size;
};

struct ns1__getAuthTypesListResponse
{
    struct ns1__AuthTypesList  result;
};

int  ns1__getAuthTypesList ( xsd__string  username,
                             xsd__string  ticket,
                             xsd__string  ipaddress,
                             struct ns1__getAuthTypesListResponse & result );


//--------------------------------------------------------------------//


#endif  // _SOAP_DEFINITIONS_H_

