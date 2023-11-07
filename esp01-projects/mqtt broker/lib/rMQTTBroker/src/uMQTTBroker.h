#ifndef _UMQTT_BROKER_H_
#define _UMQTT_BROKER_H_

#include "user_interface.h"
#include "IPAddress.h"
#include "string.h"

// the broker is composed of q MQTT server and a list of MQTT clients

#include "mqtt/mqtt_server.h"
#include "mqtt/mqtt_server_client_list.h"

#ifndef ipv4_addr_t
#define ipv4_addr_t ip_addr_t
#endif

class uMQTTBroker
{
    private:
        static uMQTTBroker *TheBroker;
        static MqttServer *TheServer;
        static MqttServerClientList *clientList;
        uint16_t _portno;
        uint16_t _max_subscriptions;
        uint16_t _max_retained_topics;

        static bool _onConnect(struct espconn *pesp_conn, uint16_t client_count);
        static void _onDisconnect(struct espconn *pesp_conn, const char *client_id);
        static bool _onAuth(const char *username, const char *password, const char *client_id, struct espconn *pesp_conn);
        static void _onData(uint32_t *args, const char *topic, uint32_t topic_len, const char *data, uint32_t length);

    public:
        uMQTTBroker(uint16_t portno, uint8_t max_subscriptions, uint8_t max_retained_topics);

        // Callbacks on client actions

        virtual bool onConnect(IPAddress addr, uint16_t client_count);
        virtual void onDisconnect(IPAddress addr, String client_id);
        virtual void onData(String topic, const char *data, uint32_t length);

        // Infos on currently connected clients

        virtual uint16_t getClientCount();
        virtual bool getClientId(uint16_t index, String &client_id);
        virtual bool getClientAddr(uint16_t index, IPAddress &addr);

        // Interaction with the local broker

        virtual bool publish(String topic, uint8_t *data, uint16_t data_length, uint8_t qos = 0, uint8_t retain = 0);
        virtual bool publish(String topic, String data, uint8_t qos = 0, uint8_t retain = 0);
        virtual bool subscribe(String topic, uint8_t qos = 0);
        virtual bool unsubscribe(String topic);

        // Cleanup all clients on Wifi connection loss

        void cleanupClientConnections();
};

#endif /* _UMQTT_BROKER_H_ */
