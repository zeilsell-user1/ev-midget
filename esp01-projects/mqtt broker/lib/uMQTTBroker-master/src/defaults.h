
#ifndef MQTT_BROKER_PORT
#define MQTT_BROKER_PORT 1883
#endif 

#ifndef MAX_SUBSCRITIONS
#define MAX_SUBSCRITIONS 30
#endif 

#ifndef MAX_RETAINED_TOPICS
#define MAX_RETAINED_TOPICS 30
#endif

#ifndef MAX_TOPIC_LIST_ENTRIES
#define MAX_TOPIC_LIST_ENTRIES 30
#endif

#ifndef MAX_TOPIC_LENGTH
#define MAX_TOPIC_LENGTH      30
#endif

#ifndef MQTT_BUF_SIZE
#define MQTT_BUF_SIZE 2048
#endif

#ifndef QUEUE_BUFFER_SIZE
#define QUEUE_BUFFER_SIZE 2048
#endif

#ifndef MAX_MQTT_CLIENTS
#define MAX_MQTT_CLIENTs 10
#endif

#ifndef MAX_SUBS_PER_REQ
#define MAX_SUBS_PER_REQ 16
#endif

#ifndef MQTT_KEEPALIVE
#define MQTT_KEEPALIVE 10 /*seconds*/
#endif

#ifndef MQTT_RECONNECT_TIMEOUT
#define MQTT_RECONNECT_TIMEOUT 5 /*seconds*/
#endif

#define PROTOCOL_NAMEv311 /*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/

#ifndef MQTT_ID
#define MQTT_ID "ESPBroker"
#endif

#undef MQTT_INFO
#define MQTT_INFO os_printf
#define MQTT_WARNING os_printf
#define MQTT_ERROR os_printf

