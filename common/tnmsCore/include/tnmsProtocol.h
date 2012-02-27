/*
 *  tnmsProtocol.h
 *
 *  Copyright(c) 2008, 2009 Timothy C. Arland
 *  Author: tca@charltontechnology.net
 */
#ifndef _TNMSCORE_TNMSPROTOCOL_H_
#define _TNMSCORE_TNMSPROTOCOL_H_


#include "tcanetpp_types.h"  // tcanetpp


namespace tnmsCore {


typedef enum TnmsErrno {
    TNMSERR_NONE         = 0,
    TNMSERR_CONFIG       = 1,
    TNMSERR_NO_CONN      = 2,
    TNMSERR_CONN_LOST    = 3,
    TNMSERR_CONN_FAIL    = 4,
    TNMSERR_CONN_DENIED  = 5
} tnmsErrno_t;


typedef enum eValueType {
    TNMS_NONE    = 0,
    TNMS_INT32   = 1,
    TNMS_UINT32  = 2,
    TNMS_INT64   = 3,
    TNMS_UINT64  = 4,
    TNMS_FLOAT   = 5,
    TNMS_STRING  = 6   // leave string as last/highest
} e_valueType_t;


typedef enum eMessageType {
    NO_RECORD,
    AUTH_REQUEST,
    AUTH_REPLY,
    AUTH_REFRESH,
    ADD_MESSAGE,
    REMOVE_MESSAGE,
    REQUEST_MESSAGE,
    METRIC_MESSAGE,
    SUBSCRIBE,
    UNSUBSCRIBE,
    SUBSCRIBE_STRUCTURE,
    UNSUBSCRIBE_STRUCTURE,
    PING_REQUEST,
    PING_REPLY
} e_msgType_t;


typedef enum eMessageOptions {
    NO_OPTIONS       = 0x00,
    LAST_MESSAGE     = 0x01,
    ZLIB_COMPRESSED  = 0x02
} e_msgOptions_t;


typedef enum eAuthType {
    AUTH_NO_RESULT,
    AUTH_SUCCESS,
    AUTH_INVALID,
    AUTH_DENIED_IP,
    AUTH_AGENT_INVALID,
    AUTH_AGENT_DENIED_IP,
    AUTH_BAD_VERSION
} e_authType_t;


typedef struct tnmsHeader
{
    uint16_t    major_version;
    uint16_t    minor_version;
    uint16_t    message_type;
    uint16_t    message_count;
    uint32_t    options;
    uint32_t    payload_size;
    uint32_t    timestamp;
} tnmsHdr_t;


typedef struct tnmsMetric
{
    uint32_t    oid_len;
    uint32_t*   oid;
    uint32_t    metric_name_len;
    char*       metric_name;
    uint16_t    metric_value_type;
    uint32_t    metric_value_len;
    char*       metric_value;
    char*       metric_value_avg;
    uint32_t    metric_samples;
    uint32_t    timestamp;
    uint32_t    pvt_data_len;
    char*       pvt_data;

    tnmsMetric()
        : oid_len(0), oid(NULL), metric_name_len(0), metric_name(NULL),
          metric_value_type(0), metric_value_len(0), metric_value(NULL),
          metric_value_avg(NULL), metric_samples(0), timestamp(0),
          pvt_data_len(0), pvt_data(NULL)
    {}

} tnmsMetric_t;


typedef struct tnmsRequest
{
    uint16_t    record_type;
    uint32_t    oid_len;
    uint32_t*   oid;
    uint32_t    metric_name_len;
    char*       metric_name;

    tnmsRequest ( int type )
        : record_type(type)
    {}
} tnmsRequest_t;


typedef struct tnmsAdd : public tnmsRequest
{
    tnmsAdd()
        : tnmsRequest(ADD_MESSAGE)
    {}
} tnmsAdd_t;


typedef struct tnmsRemove : public tnmsRequest
{
    tnmsRemove()
        : tnmsRequest(REMOVE_MESSAGE)
    {}
} tnmsRemove_t;


typedef struct tnmsSubscribe : public tnmsRequest
{
    tnmsSubscribe()
        : tnmsRequest(SUBSCRIBE)
    {}
} tnmsSub_t;


typedef struct tnmsUnsubscribe : public tnmsRequest
{
    tnmsUnsubscribe()
        : tnmsRequest(UNSUBSCRIBE)
    {}
} tnmsUnsub_t;


typedef struct tnmsAuthRequest
{
    uint32_t    agent_name_len;
    char*       agent_name;
    uint32_t    auth_key_len;
    char*       auth_key;
    uint32_t    ip_address;
} tnmsAuth_t;


typedef struct tnmsAuthReply
{
    uint32_t    agent_name_len;
    char*       agent_name;
    uint32_t    oid_len;
    uint32_t*   oid;
    uint16_t    auth_result;
    uint32_t    reason_len;
    char*       reason;
    uint32_t    reply_data_len;
    char*       reply_data;
} tnmsAuthReply_t;


}  // namespace


#endif  //  _TNMSCORE_TNMSPROTOCOL_H_

