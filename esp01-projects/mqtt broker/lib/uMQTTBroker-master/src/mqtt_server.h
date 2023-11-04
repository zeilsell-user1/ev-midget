#ifndef _MQTT_SERVER_H_
#define _MQTT_SERVER_H_

#include "user_interface.h"
#include "mqtt/mqtt_topics.h"

class MqttServer
{
  private:
    // void print_topic(topic_entry *topic) const;
    // bool MqttServer::publish_topic(topic_entry *topic_e, uint8_t *topic, uint8_t *data, uint16_t data_len) const;

public:
    typedef bool (*MqttConnectCallback)(struct espconn *pesp_conn, uint16_t client_count);
    typedef bool (*MqttDisconnectCallback)(struct espconn *pesp_conn, const char *client_id);

    MqttServer(uint16_t portno);
    
    void TcpConnectedCb(void *arg);
    void TcpReceiveCb(void *arg, char *pdata, unsigned short len);
    void TcpDisconnectCb(void *arg);
    void TcpSentCb(void *arg);

    // uint16_t MQTT_server_countClientCon();
    // char *MQTT_server_getClientId(uint16_t index);
    // struct espconn *MQTT_server_getClientPcon(uint16_t index);

    // bool MQTT_server_start(uint16_t portno, uint16_t max_subscriptions, uint16_t max_retained_topics);
    // void MQTT_server_onConnect(MqttConnectCallback connectCb);
    // void MQTT_server_onDisconnect(MqttDisconnectCallback connectCb);
    // void MQTT_server_onData(MqttDataCallback dataCb);
    // bool MQTT_local_publish(uint8_t *topic, uint8_t *data, uint16_t data_length, uint8_t qos, uint8_t retain);
    // bool MQTT_local_subscribe(uint8_t *topic, uint8_t qos);
    // bool MQTT_local_unsubscribe(uint8_t *topic);
};

#endif /* _MQTT_SERVER_H_ */
