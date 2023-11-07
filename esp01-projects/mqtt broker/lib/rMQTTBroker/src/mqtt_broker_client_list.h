#ifndef _MQTT_SERVER_CLIENT_LIST_H_
#define _MQTT_SERVER_CLIENT_LIST_H_

#include "user_interface.h"
#include "mqtt_server_client.h"

#ifndef ipv4_addr_t
#define ipv4_addr_t ip_addr_t
#endif

class MqttServerClientList // was _MQTT_ClientCon
{
    private:
        MqttServerClient client_list[MAX_MQTT_CLIENTS];

    public:
        MqttServerClientList();
        void init();
        void DisconnectClient(MQTT_ClientCon *mqttClientCon);
        bool DeleteClientCon(MQTT_ClientCon *mqttClientCon);
        void CleanupClientCons();
};

#endif /* _MQTT_SERVER_CLIENT_LIST_H_ */

