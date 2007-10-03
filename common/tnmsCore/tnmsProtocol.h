#ifndef TNMSPROTOCOL_H_
#define TNMSPROTOCOL_H_


namespace tnmscore {


enum tnmsValueTypes {
    TNMS_NONE,
    TNMS_INT,
    TNMS_UINT,
    TNMS_LONG,
    TNMS_ULONG,
    TNMS_FLOAT,
    TNMS_STRING
};

enum tnmsRecordTypes {
    NO_RECORD,
    AUTH_REQUEST,
    AUTH_REPLY,
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
};

enum tnmsAuthResult {
    AUTH_NO_RESULT,
    AUTH_SUCCESS,
    AUTH_INVALID,
    AUTH_DENIED_IP,
    AUTH_AGENT_INVALID,
    AUTH_AGENT_DENIED_IP,
    AUTH_BAD_VERSION
};

    
struct tnmsHeader
{
    uint16_t  major_version;
    uint16_t  minor_version;
    uint16_t  record_type;
    uint16_t  record_count;
    uint32_t  payload_size;
    uint32_t  timestamp;
};

struct tnmsMetric 
{
    uint16_t    oid_len;
    uint16_t*   oid;
    uint8_t     metric_value_type;
    uint8_t     pad;
    uint32_t    timestamp;
    uint32_t    metric_value_len;
    char*       metric_value;
    uint32_t    data_len;
    char*       data;
};

struct tnmsAddRecord 
{
    uint32_t    metric_name_len;
    char*       metric_name;
    uint16_t    oid_len;
    uint16_t*   oid;
};

struct tnmsRemoveRecord 
{
    uint16_t    oid_len;
    uint16_t*   oid;
};

struct tnmsAuthRequest
{
    uint32_t    agent_name_len;
    char*       agent_name;
    uint32_t    auth_key_len;
    char*       auth_key;
    uint32_t    ip_address;
};

struct tnmsAuthReply
{
    uint32_t    agent_name_len;
    char*       agent_name;
    uint8_t     auth_result;
    uint8_t     pad;
    uint32_t    reason_len;
    char*       reason;
    uint32_t    reply_data_len;
    char*       reply_data;
};

struct tnmsRecordRequest
{
    uint16_t    oid_len;
    uint16_t*   oid;
};


}  // namespace

#endif /*TNMSPROTOCOL_H_*/
