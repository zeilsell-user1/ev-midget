#include "mqtt/mqtt_server_client.h"
#include "mqtt/debug.h"

MqttServerClient::MqttServerClient()
{
	MQTT_INFO("MQTT: instantiate MqttServerClient\r\n");

	connState = TCP_CONNECTED;
	os_memset(&connect_info, 0, sizeof(mqtt_connect_info_t));
	os_memset(&mqtt_state.in_buffer, 0, MQTT_BUF_SIZE);
	connect_info.client_id = zero_len_id;
	protocolVersion = 0;
	mqtt_state.in_buffer_length = MQTT_BUF_SIZE;
	mqtt_state.connect_info = &connect_info;
	mqtt_msg_init(&mqtt_state.mqtt_connection, mqtt_state.out_buffer, mqtt_state.out_buffer_length);
	QUEUE_Init(&msgQueue, QUEUE_BUFFER_SIZE);
	clientcon_list = mqttClientCon;
}

bool deleteClientCon()
{
	MQTT_INFO("MQTT: DeleteClientCon\r\n");

	if (mqttClientCon == NULL)
		return true;

	os_timer_disarm(&mqttTimer);

	MQTT_ClientCon **p = &clientcon_list;

	while (*p != mqttClientCon && *p != NULL)
	{
		p = &((*p)->next);
	}

	if (*p == mqttClientCon)
		*p = (*p)->next;

		os_free(user_data);
		user_data = NULL;
		os_free(mqtt_state.in_buffer);
		mqtt_state.in_buffer = NULL;
		mqtt_state.mqtt_connection.buffer = NULL;

	if (connect_info.client_id != NULL)
	{
		/* Don't attempt to free if it's the zero_len array */
		if (((uint8_t *)connect_info.client_id) != zero_len_id)
		{
			if (concrete_disconnect_cb != NULL)
			{
				concrete_disconnect_cb(pCon, connect_info.client_id);
			}
			os_free(connect_info.client_id);
		}

		connect_info.client_id = NULL;
	}

		os_free(connect_info.username);
		connect_info.username = NULL;
		os_free(connect_info.password);
		connect_info.password = NULL;


	if (connect_info.will_topic != NULL)
	{
		// Publish the LWT
		find_topic(connect_info.will_topic, publish_topic,
				   connect_info.will_data, connect_info.will_data_len);
		activate_next_client();

		if (connect_info.will_retain)
		{
			update_retainedtopic(connect_info.will_topic, connect_info.will_data,
								 connect_info.will_data_len, connect_info.will_qos);
		}

		os_free(connect_info.will_topic);
		connect_info.will_topic = NULL;
	}
		os_free(connect_info.will_data);
		connect_info.will_data = NULL;
		os_free(msgQueue.buf);
		msgQueue.buf = NULL;


	delete_topic(mqttClientCon, 0);

	os_free(mqttClientCon);

	return true;
}
