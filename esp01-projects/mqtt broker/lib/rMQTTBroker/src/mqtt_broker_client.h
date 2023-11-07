#ifndef _MQTT_SERVER_CLIENT_H_
#define _MQTT_SERVER_CLIENT_H_

#include "user_interface.h"
#include "IPAddress.h"
#include "string.h"
#include "mqtt_server_msg.h"

#ifndef ipv4_addr_t
#define ipv4_addr_t ip_addr_t
#endif

class MqttServerClient // was _MQTT_ClientCon
{
    typedef struct mqtt_connect_info
    {
        uint8_t* client_id;
        char* will_topic;
        char* will_data;
        uint16_t will_data_len;
        uint32_t keepalive;
        int will_qos;
        int will_retain;
        int clean_session;
    } mqtt_connect_info_t;

    typedef struct mqtt_state_t
    {
        uint16_t port;
        int auto_reconnect;
        mqtt_connect_info_t* connect_info;
        uint8_t in_buffer[MQTT_BUF_SIZE];
        uint8_t out_buffer[MQTT_BUF_SIZE];
        int in_buffer_length;
        int out_buffer_length;
        uint16_t message_length;
        uint16_t message_length_read;
        MqttServerMsg::mqtt_connection_t mqtt_connection;
        uint16_t pending_msg_id;
        int pending_msg_type;
        int pending_publish_qos;
    } mqtt_state_t;

    typedef enum {
        WIFI_INIT,
        WIFI_CONNECTING,
        WIFI_CONNECTING_ERROR,
        WIFI_CONNECTED,
        DNS_RESOLVE,
        TCP_DISCONNECTING,
        TCP_DISCONNECT,
        TCP_DISCONNECTED,
        TCP_CLIENT_DISCONNECTED,
        TCP_RECONNECT_DISCONNECTING,
        TCP_RECONNECT_REQ,
        TCP_RECONNECT,
        TCP_CONNECTING,
        TCP_CONNECTING_ERROR,
        TCP_CONNECTED,
        MQTT_CONNECT_SEND,
        MQTT_CONNECT_SENDING,
        MQTT_SUBSCIBE_SEND,
        MQTT_SUBSCIBE_SENDING,
        MQTT_DATA,
        MQTT_KEEPALIVE_SEND,
        MQTT_PUBLISH_RECV,
        MQTT_PUBLISHING,
        MQTT_DELETING,
        MQTT_DELETED,
    } tConnState;
    
    uint8_t zero_len_id[2] = {0, 0};

    private:
        ip_addr_t ip;                       // the IP address of the client
        uint32_t port;                      // the port used by the client
        tConnState connState;               // the state of the client
        mqtt_connect_info_t connect_info;   

        struct espconn *pCon;
        mqtt_state_t mqtt_state;
         ETSTimer mqttTimer;
        uint32_t sendTimeout;
        uint32_t connectionTimeout;
        QUEUE msgQueue;
        uint8_t protocolVersion;
        void *user_data;

    public:
        MqttServerClient();

        void init();
        bool deleteClientCon();
};

#endif /* _MQTT_SERVER_CLIENT_H_ */

