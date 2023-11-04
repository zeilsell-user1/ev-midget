#ifndef _MQTT_BROKER_TOPIC_LIST_H_
#define _MQTT_BROKER_TOPIC_LIST_H_

#include "c_types.h"
#include "defaults.h"
#include "mqtt_server_topics.h"

class MqttBrokerTopicList {
    MqttBrokerTopicList();

    public:
        typedef struct _topic_entry {
            int8_t  connectionIndex;
            uint8_t topic[MAX_TOPIC_LENGTH];
            int8_t  qos;
        } topic_entry;

    typedef bool (*iterate_topic_cb)(topic_entry *topic, void *user_data);
    typedef bool (*find_topic_cb)(topic_entry *topic_e, uint8_t *topic, uint8_t *data, uint16_t data_len);

    bool add_topic(uint8_t connectionIndex, uint8_t *topic, uint8_t qos);
    bool delete_topic(uint8_t connectionIndex, uint8_t *topic);
    bool find_topic(uint8_t *topic, find_topic_cb cb, uint8_t *data, uint16_t data_len);
    void iterate_topics(iterate_topic_cb cb, void *user_data); 
    
    private:
        topic_entry topic_list[MAX_TOPIC_LIST_ENTRIES];
        uint16_t max_entry;
};

#endif /* _MQTT_BROKER_TOPIC_LIST_H_ */
