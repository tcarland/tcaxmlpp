#ifndef _TNMSCORE_TNMSPROTOCOL_H_
#define _TNMSCORE_TNMSPROTOCOL_H_

#include "tcanetpp_types.h"


namespace tnmscore {


#define TNMS_VERSION_MAJOR          0x05
#define TNMS_VERSION_MINOR          0x61

#define TNMS_DEFAULT_DELIMITER      "."
#define TNMS_RECORD_LIMIT           1000
#define TNMS_RECORD_SIZE            1024
#define TNMS_PACKET_SIZE            (TNMS_RECORD_SIZE * MAX_QUEUE_LIMIT)

#define TNMS_FLUSH_LIMIT            500
#define TNMS_CLIENT_TIMEOUT         120

#define DEFAULT_COMPRESSION_ENABLE  1
#define DEFAULT_FLUSH_ENABLE        1


typedef enum eValueTypes {
    TNMS_NONE    = 0,
    TNMS_INT32   = 1,
    TNMS_UINT32  = 2,
    TNMS_INT64   = 3,
    TNMS_UINT64  = 4,
    TNMS_FLOAT   = 5,
    TNMS_STRING  = 6
} e_valueTypes_t;


typedef enum eRecordTypes {
    NO_RECORD,
    AUTH_REQUEST,
    AUTH_REPLY,
    AUTH_REFRESH,
    RECORD_ADD,
    RECORD_REMOVE,
    RECORD_REQUEST,
    RECORD_METRIC,
    SUBSCRIBE,
    UNSUBSCRIBE,
    SUBSCRIBE_STRUCTURE,
    UNSUBSCRIBE_STRUCTURE,
    PING_REQUEST,
    PING_REPLY
} e_recordTypes_t;


typedef enum eAuthTypes {
    AUTH_NO_RESULT,
    AUTH_SUCCESS,
    AUTH_INVALID,
    AUTH_DENIED_IP,
    AUTH_AGENT_INVALID,
    AUTH_AGENT_DENIED_IP,
    AUTH_BAD_VERSION
} e_authTypes_t;


typedef struct tnmsHeader
{
    uint16_t    major_version;
    uint16_t    minor_version;
    uint16_t    record_type;
    uint16_t    record_count;
    uint32_t    options;
    uint32_t    payload_size;
    uint32_t    timestamp;
} tnmsHdr_t;


typedef struct tnmsMetric
{
    uint32_t    oid_len;
    uint16_t*   oid;
    uint32_t    metric_name_len;
    char*       metric_name;
    uint16_t    metric_value_type;
    uint32_t    metric_value_len;
    char*       metric_value;
    uint32_t    pvt_data_len;
    char*       pvt_data;
} tnmsMetric_t;


typedef struct tnmsRequest
{
    uint16_t    record_type;    
    uint32_t    oid_len;
    uint16_t*   oid;
    uint32_t    metric_name_len;
    char*       metric_name;

    tnmsRequest ( int type )
        : record_type(type)
    {}
} tnmsRequest_t;


typedef struct tnmsAdd : public tnmsRequest
{
    tnmsAdd() 
        : tnmsRequest(RECORD_ADD)
    {}
} tnmsAdd_t;


typedef struct tnmsRemove : public tnmsRequest
{
    tnmsRemove()
        : tnmsRequest(RECORD_REMOVE)
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
    uint16_t*   oid;
    uint16_t    auth_result;
    uint32_t    reason_len;
    char*       reason;
    uint32_t    reply_data_len;
    char*       reply_data;
} tnmsAuthReply_t;


}  // namespace


#endif  //  _TNMSCORE_TNMSPROTOCOL_H_
