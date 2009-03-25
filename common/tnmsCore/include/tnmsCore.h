/*
 * tnmsCore.h
 *
 *  Copyright(c) 2008, Charlton Technology LLC
 *      Author: tcarland@gmail.com
 */
#ifndef _TNMSCORE_H_
#define _TNMSCORE_H_


#define TNMS_VERSION_MAJOR         0x0000
#define TNMS_VERSION_MINOR         0x0001
#define TNMS_VERSION               ( (((uint32_t)TNMS_VERSION_MAJOR << 16) & 0xffff0000) | \
                                      ((uint32_t)TNMS_VERSION_MINOR) )
#define TNMS_GET_MAJOR(x)          (  ((uint32_t)x >> 16) & 0x0000ffff )
#define TNMS_GET_MINOR(x)          (   (uint32_t)x & 0x0000ffff )

#define TNMS_DELIMITER             "/"
#define TNMS_DELIMITER_CHAR        '/'

#define TNMS_RECORD_LIMIT          1000        // max records
#define TNMS_RECORD_SIZE           1024        // avg rec size

#define TNMS_PACKET_SIZE           ( TNMS_RECORD_SIZE * TNMS_RECORD_LIMIT )
#define TNMS_MAX_PACKET_SIZE       ( TNMS_PACKET_SIZE * 2 )

#define TNMS_FLUSH_LIMIT           500         // minimum flush amt
#define TNMS_CLIENT_TIMEOUT        120         // stall timeout
#define TNMS_LOGIN_ATTEMPTS        5
#define TNMS_COMPRESSION_ENABLE    1
#define TNMS_FLUSH_ENABLE          1


#include "tnmsProtocol.h"
#include "TnmsSocket.h"


#endif //  _TNMSCORE_H_

