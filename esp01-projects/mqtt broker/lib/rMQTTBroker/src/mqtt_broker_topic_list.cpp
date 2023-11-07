#include "c_types.h"
#include "mem.h"
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"

#include <string.h>

#include "mqtt_broker_topic_list.h"
#include "mqtt_server_topics.h"

MqttBrokerTopicList::MqttBrokerTopicList()
{
	max_entry = MAX_TOPIC_LIST_ENTRIES;

	topic_entry null_topic;
	null_topic.connectionIndex = -1;
	strnset(null_topic.topic, '\0', MAX_TOPIC_LENGTH);
	null_topic.qos = -1;

	for (uint8_t i = 0; i < MAX_TOPIC_LIST_ENTRIES; i++)
	{
		topic_list[i] = null_topic;
	}
}

bool MqttBrokerTopicList::add_topic(uint8_t connectionIndex, uint8_t *topic, uint8_t qos)
{
	uint16_t i;

	if (!MqttServerTopics::Topics_isValidName((char *)topic))
	{
		return false;
	}

	for (i = 0; i < MqttBrokerTopicList::max_entry; i++)
	{
		if (topic_list[i].connectionIndex == -1)
		{
			os_strncpy((char *)topic_list[i].topic, (char *)topic, MAX_TOPIC_LENGTH);
			topic_list[i].connectionIndex = connectionIndex;
			topic_list[i].qos = qos;
			return true;
		}
	}
	return false;
}

bool MqttBrokerTopicList::delete_topic(uint8_t connectionIndex, uint8_t *topic)
{
	uint16_t i;

	for (i = 0; i < max_entry; i++)
	{
		if (topic_list[i].connectionIndex == connectionIndex))
		{
			topic_list[1].connectionIndex = -1;
			topic_list[1].
		}
	}
	return true;
}

bool find_topic(uint8_t *topic, find_topic_cb cb, uint8_t *data, uint16_t data_len)
{
	uint16_t i;
	bool retval = false;

	if (topic_list == NULL)
		return false;

	for (i = 0; i < max_entry; i++)
	{
		if (topic_list[i].clientcon != NULL)
		{
			if (Topics_matches(topic_list[i].topic, 1, topic))
			{
				(*cb)(&topic_list[i], topic, data, data_len);
				retval = true;
			}
		}
	}
	return retval;
}

void iterate_topics(iterate_topic_cb cb, void *user_data)
{
	uint16_t i;

	if (topic_list == NULL)
		return;

	for (i = 0; i < max_entry; i++)
	{
		if (topic_list[i].clientcon != NULL)
		{
			if ((*cb)(&topic_list[i], user_data) == true)
				return;
		}
	}
}
