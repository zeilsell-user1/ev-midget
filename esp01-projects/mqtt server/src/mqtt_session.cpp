/*******************************************************************************
 * Copyright (c) 2023 George Consultants Ltd.
 * richard.john.george.3@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the “Software”), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *******************************************************************************/

#include "mqtt_session.h"
#include "mqtt_message_handler.h"

/*
 ******************************************************************************
 * Callback functions to receive data and events from the TCP server. These
 * are outside the class so that the TCP server remains indifferent to the
 * deatils of the client calling it.
 ******************************************************************************
 */

void sessionDisconnectCb(void *args, void *obj)
{
    MqttSession *mqttSession = (MqttSession *)(obj);
    mqttSession->handleTcpDisconnect(args);
}

void messageSentCb(void *args, void *obj)
{
    MqttSession *mqttSession = (MqttSession *)(obj);
    mqttSession->handleTcpMessageSent(args);
}

void incomingMessageCb(void *args, char *pdata, unsigned short len, void *obj)
{
    MqttSession *mqttSession = (MqttSession *)(obj);
    mqttSession->handleTcpIncomingMessage(args, pdata, len);
}

/*
 ******************************************************************************
 * Public methods
 ******************************************************************************
 */

MqttSession::MqttSession()
{
}

MqttSession::MqttSession(std::shared_ptr<TcpSession> tcpSession)
{
    tcpSession = tcpSession;
    tcpSession->registerSessionCbListener((void *)this);
    tcpSession->registerSessionDisconnectCb(sessionDisconnectCb);
    tcpSession->registerIncomingMessageCb(incomingMessageCb);
    tcpSession->registerMessageSentCb(messageSentCb);
}

void MqttSession::setSessionFalse()
{
    sessionValid_ = false;
}

bool MqttSession::isSessionValid()
{
    return sessionValid_;
}

std::shared_ptr<TcpSession> MqttSession::getTcpSession()
{
    return tcpSession_;
}

/*
 * ****************************************************************************
 * Methods used to handle the events from the TCP session
 * ****************************************************************************
 */

void MqttSession::handleTcpDisconnect(void *arg)
{

}

void MqttSession::handleTcpMessageSent(void *arg)
{
}

void MqttSession::handleTcpIncomingMessage(void *arg, char *pdata, unsigned short len)
{
    MqttMessageHandler::handleMessage((const unsigned char *)pdata, len);
}

/*
 * ****************************************************************************
 * MQTT Session State MAchine
 * ****************************************************************************
 */

// State Machine:

// State: WAIT_FOR_CONNECT
// Event: Client sends a CONNECT packet
// Actions:
//     - Parse the CONNECT packet
//     - If valid, transition to STATE_CONNECTED
//     - If invalid, send a CONNACK with an appropriate return code and transition to STATE_DISCONNECTED


void MqttSession::WaitForConnect_HandleMsg(MqttMessage msg)
{

}

// State: STATE_CONNECTED
// Event: Client sends a PUBLISH packet
// Actions:
//     - Process the PUBLISH message
//     - Optionally, publish the message to subscribed clients (if QoS > 0)
//     - If QoS = 1, send a PUBACK
//     - If QoS = 2, send a PUBREC and transition to STATE_WAIT_FOR_PUBREL

// Event: Client sends a SUBSCRIBE packet
// Actions:
//     - Process the SUBSCRIBE message
//     - Add the client to the list of subscribers for the requested topics
//     - Send a SUBACK

// Event: Client sends an UNSUBSCRIBE packet
// Actions:
//     - Process the UNSUBSCRIBE message
//     - Remove the client from the list of subscribers for the requested topics
//     - Send an UNSUBACK

// Event: Client sends a DISCONNECT packet
// Actions:
//     - Clean up any resources associated with the client
//     - Send a DISCONNECT packet (optional)
//     - Transition to STATE_DISCONNECTED

// Event: Client sends a PUBACK packet (QoS = 1)
// Actions:
//     - Process the PUBACK message
//     - Optionally, retry sending any pending PUBLISH messages

void MqttSession::Connected_HandleMsg(MqttMessage msg)
{

}

// State: STATE_WAIT_FOR_PUBREL
// Event: Client sends a PUBREL packet (QoS = 2)
// Actions:
//     - Process the PUBREL message
//     - Send a PUBCOMP
//     - Transition to STATE_CONNECTED

void MqttSession::WaitForPubRel_HandleMsg(MqttMessage msg)
{

}

// State: STATE_DISCONNECTED
// Event: Client re-establishes connection
// Actions:
//     - Send a CONNACK with a return code indicating successful reconnection
//     - Transition to STATE_CONNECTED

// Event: Client disconnects
//     Actions:
//     - Clean up any resources associated with the client

// Event: Timeout waiting for client activity
//     Actions:
//         - Close the connection and free any associated resources


void MqttSession::Disconnected_HandleMsg(MqttMessage msg)
{

}











// #include <espconn.h>
// #include "mqtt_server.h"
// #include "debug.h"
// #include "defaults.h"

// #define MQTT_SERVER_TASK_PRIO 1
// #define MQTT_TASK_QUEUE_SIZE 1
// #define MQTT_SEND_TIMOUT 5

// os_event_t mqtt_procServerTaskQueue[MQTT_TASK_QUEUE_SIZE];

// MqttDataCallback concrete_data_cb = NULL;
// MqttConnectCallback concrete_connect_cb = NULL;
// MqttDisconnectCallback concrete_disconnect_cb = NULL;

// static uint8_t shared_out_buffer[MQTT_BUF_SIZE];

// #define MINIMUM_SUBSCRIBE_LENGTH 8

// MqttSession::MqttSession(uint16_t portno)
// {
// 	MQTT_INFO("Starting MQTT server on port %d\r\n", portno);

// 	struct espconn serverConfig;
// 	esp_tcp tcpConfig;
// 	serverConfig.type = ESPCONN_TCP;
// 	serverConfig.state = ESPCONN_NONE;
// 	serverConfig.proto.tcp = &tcpConfig;
// 	serverConfig.proto.tcp->local_port = portno;

// 	espconn_regist_connectcb(&serverConfig, TcpConnectedCb);
// 	espconn_accept(&serverConfig);

// 	system_os_task(MQTT_ServerTask, MQTT_SERVER_TASK_PRIO, mqtt_procServerTaskQueue, MQTT_TASK_QUEUE_SIZE);
// }

// void MqttSession::TcpConnectedCb(void *arg)
// {
// 	struct espconn *pespconn = (struct espconn *)arg;
// 	MQTT_ClientCon *mqttClientCon;
// 	pespconn->reverse = NULL;

// 	MQTT_INFO("MQTT_ClientCon_connected_cb(): Client connected\r\n");

// 	espconn_regist_sentcb(pespconn, (espconn_sent_callback)TcpSentCb);
// 	espconn_regist_disconcb(pespconn, (espconn_connect_callback)TcpDisconnectCb);
// 	espconn_regist_recvcb(pespconn, (espconn_recv_callback)TcpReceiveCb);
// 	espconn_regist_time(pespconn, 20, 1); // RJG set the TCP timeout to 20 seconds from 30. MQTT keepalive is 10 seconds

// 	mqttClientCon = (MQTT_ClientCon *)os_zalloc(sizeof(MQTT_ClientCon));
// 	pespconn->reverse = mqttClientCon;


// 	mqttClientCon->pCon = pespconn;

// 	bool no_mem = (system_get_free_heap_size() < (MQTT_BUF_SIZE + QUEUE_BUFFER_SIZE + 0x400));
// 	if (no_mem)
// 	{
// 		MQTT_ERROR("ERROR: No mem for new client connection\r\n");
// 	}

// 	if (no_mem || (concrete_connect_cb != NULL && concrete_connect_cb(pespconn, MQTT_server_countClientCon() + 1) == false) )
// 	{
// 		mqttClientCon->connState = TCP_DISCONNECT;
// 		system_os_post(MQTT_SERVER_TASK_PRIO, 0, (os_param_t)mqttClientCon);
// 		return;
// 	}

// 	MQTT_server_initClientCon(mqttClientCon);

// 	mqttClientCon->connectionTimeout = 40;
// 	os_timer_setfn(&mqttClientCon->mqttTimer, (os_timer_func_t *)mqtt_server_timer, mqttClientCon);
// 	os_timer_arm(&mqttClientCon->mqttTimer, 1000, 1);
// }

// void MqttSession::TcpReceiveCb(void *arg, char *pdata, unsigned short len)
// {
// 	uint8_t msg_type;
// 	uint8_t msg_qos;
// 	uint16_t msg_id;
// 	enum mqtt_connect_flag msg_conn_ret;
// 	uint16_t topic_index;
// 	uint16_t topic_len;
// 	uint8_t *topic_str;
// 	uint8_t topic_buffer[MQTT_BUF_SIZE];
// 	uint16_t data_len;
// 	uint8_t *data;

// 	struct espconn *pCon = (struct espconn *)arg;

// 	MQTT_INFO("MQTT_ClientCon_recv_cb(): %d bytes of data received\r\n", len);

// 	MQTT_ClientCon *clientcon = (MQTT_ClientCon *)pCon->reverse;
// 	if (clientcon == NULL)
// 	{
// 		MQTT_ERROR("ERROR: No client status\r\n");
// 		return;
// 	}

// 	MQTT_INFO("MQTT: TCP: data received %d bytes (State: %d)\r\n", len, clientcon->connState);
// 	MQTT_INFO("MQTT: clientcon->mqtt_state.message_length_read %d\r\n", clientcon->mqtt_state.message_length_read);

// 	// Do not exceed receive buffer
// 	if (len + clientcon->mqtt_state.message_length_read > MQTT_BUF_SIZE)
// 	{
// 		MQTT_ERROR("MQTT: Message too long: %d\r\n", len + clientcon->mqtt_state.message_length_read);
// 		MQTT_server_disconnectClientCon(clientcon);
// 		// clientcon->mqtt_state.message_length_read = 0;
// 		return;
// 	}

// 	os_memcpy(&clientcon->mqtt_state.in_buffer[clientcon->mqtt_state.message_length_read], pdata, len);
// 	clientcon->mqtt_state.message_length_read += len;

// READPACKET:
// 	// Expect minimum the full fixed size header
// 	if (len < 2)
// 	{
// 		MQTT_INFO("MQTT: Partial message received (< 2 byte)\r\n");
// 		return;
// 	}
// 	clientcon->mqtt_state.message_length =
// 		mqtt_get_total_length(clientcon->mqtt_state.in_buffer, clientcon->mqtt_state.message_length_read);
// 	MQTT_INFO("MQTT: next message length: %d\r\n", clientcon->mqtt_state.message_length);
// 	if (clientcon->mqtt_state.message_length > clientcon->mqtt_state.message_length_read)
// 	{
// 		MQTT_INFO("MQTT: Partial message received (%d vs. %d\r\n", clientcon->mqtt_state.message_length,
// 				  clientcon->mqtt_state.message_length_read);
// 		return;
// 	}

// 	msg_type = mqtt_get_type(clientcon->mqtt_state.in_buffer);
// 	MQTT_INFO("MQTT: message_type: %d\r\n", msg_type);
// 	// msg_qos = mqtt_get_qos(clientcon->mqtt_state.in_buffer);
// 	switch (clientcon->connState)
// 	{
// 	case TCP_CONNECTED:
// 		MQTT_ClientCon_recv_TCP_Connect(clientcon, msg_type, msg_conn_ret);
// 		break;

// 	case MQTT_DATA:

// 		break;
// 	}

// 	clientcon->connectionTimeout = 2 * clientcon->connect_info.keepalive + 10;

// 	// More than one MQTT command in the packet?
// 	int diff = clientcon->mqtt_state.message_length_read - clientcon->mqtt_state.message_length;
// 	if (diff > 0)
// 	{
// 		MQTT_INFO("MQTT: %d bytes left in buffer, try next message\r\n", diff);
// 		os_memcpy(clientcon->mqtt_state.in_buffer, &clientcon->mqtt_state.in_buffer[clientcon->mqtt_state.message_length], diff);
// 		clientcon->mqtt_state.message_length_read = diff;
// 		goto READPACKET;
// 	}
// 	clientcon->mqtt_state.message_length_read = 0;

// 	if (msg_type != MQTT_MSG_TYPE_PUBLISH)
// 	{
// 		system_os_post(MQTT_SERVER_TASK_PRIO, 0, (os_param_t)clientcon);
// 	}
// 	else
// 	{
// 		activate_next_client();
// 	}
// }

// void MqttSession::TcpDisconnectCb(void *arg)
// {
// 	struct espconn *pCon = (struct espconn *)arg;
// 	MQTT_ClientCon *clientcon = (MQTT_ClientCon *)pCon->reverse;

// 	MQTT_INFO("MQTT_ClientCon_discon_cb(): client disconnected\r\n");

// 	if (clientcon != &disconnecting_clientcon)
// 	{
// 		MQTT_server_deleteClientCon(clientcon);
// 	}
// 	else
// 	{
// 		clientcon->pCon = NULL;
// 	}
// }

// void MqttSession::TcpSentCb(void *arg)
// {
// 	struct espconn *pCon = (struct espconn *)arg;
// 	MQTT_ClientCon *clientcon = (MQTT_ClientCon *)pCon->reverse;

// 	MQTT_INFO("MQTT_ClientCon_sent_cb(): Data sent\r\n");

// 	clientcon->sendTimeout = 0;

// 	if (clientcon->connState == TCP_DISCONNECTING)
// 	{
// 		MQTT_server_disconnectClientCon(clientcon);
// 	}

// 	activate_next_client();
// }










// void MqttSession::recvTcpConnect( uint8_t msg_type, enum mqtt_connect_flag msg_conn_ret)
// {
// 	switch (msg_type)
// 	{
// 	case MQTT_MSG_TYPE_CONNECT:

// 		MQTT_INFO("MQTT: Connect received, message_len: %d\r\n", clientcon->mqtt_state.message_length, msg_conn_ret);
// 		msg_conn_ret = MQTT_ClientCon_recv_MQTT_CONNECT(clientcon);

// 		MQTT_INFO("send CONNACK");
// 		clientcon->mqtt_state.outbound_message = mqtt_msg_connack(&clientcon->mqtt_state.mqtt_connection, msg_conn_ret);
// 		if (QUEUE_Puts(&clientcon->msgQueue, clientcon->mqtt_state.outbound_message->data,
// 					   clientcon->mqtt_state.outbound_message->length) == -1)
// 		{
// 			MQTT_ERROR("MQTT: Queue full\r\n");
// 		}
// 		break;

// 	default:
// 		MQTT_WARNING("MQTT: Invalid message\r\n");
// 		MQTT_server_disconnectClientCon(clientcon);
// 		break;
// 	}

// 	return;
// }

// enum mqtt_connect_flag MqttSession::recvMqttConnect()
// {
// 	if (clientcon->mqtt_state.message_length < sizeof(struct mqtt_connect_variable_header) + 3)
// 	{
// 		MQTT_ERROR("MQTT: Too short Connect message\r\n");
// 		MQTT_server_disconnectClientCon(clientcon);
// 		return (CONNECTION_REFUSE_ID_REJECTED);
// 	}

// 	struct mqtt_connect_variable_header4 *variable_header =
// 		(struct mqtt_connect_variable_header4 *)&clientcon->mqtt_state.in_buffer[2];
// 	uint16_t var_header_len = sizeof(struct mqtt_connect_variable_header4);

// 	clientcon->protocolVersion = 4;

// 	uint16_t msg_used_len = var_header_len;

// 	MQTT_INFO("MQTT: Connect flags %x\r\n", variable_header->flags);
// 	clientcon->connect_info.clean_session = (variable_header->flags & MQTT_CONNECT_FLAG_CLEAN_SESSION) != 0;

// 	clientcon->connect_info.keepalive = (variable_header->keepaliveMsb << 8) + variable_header->keepaliveLsb;
// 	espconn_regist_time(clientcon->pCon, 2 * clientcon->connect_info.keepalive, 1);
// 	MQTT_INFO("MQTT: Keepalive %d\r\n", clientcon->connect_info.keepalive);

// 	// Get the client id
// 	uint16_t id_len = clientcon->mqtt_state.message_length - (2 + msg_used_len);
// 	const char *client_id = mqtt_get_str(&clientcon->mqtt_state.in_buffer[2 + msg_used_len], &id_len);
// 	if (client_id == NULL || id_len > 80 || id_len == 0)
// 	{
// 		MQTT_WARNING("MQTT: Client Id invalid\r\n");
// 		clientcon->connState = TCP_DISCONNECTING;
// 		return CONNECTION_REFUSE_ID_REJECTED;
// 	}
// 	else
// 	{
// 		uint8_t *new_id = (char *)os_zalloc(id_len + 1);
// 		if (new_id == NULL)
// 		{
// 			MQTT_ERROR("MQTT: Out of mem\r\n");
// 			clientcon->connState = TCP_DISCONNECTING;
// 			return CONNECTION_REFUSE_SERVER_UNAVAILABLE;
// 		}
// 		os_memcpy(new_id, client_id, id_len);
// 		new_id[id_len] = '\0';

// 		// Delete any existing status for that id
// 		delete_client_by_id(client_id);

// 		clientcon->connect_info.client_id = new_id;
// 	}
// 	MQTT_INFO("MQTT: Client id \"%s\"\r\n", clientcon->connect_info.client_id);
// 	msg_used_len += 2 + id_len;

// 	// Get the LWT
// 	clientcon->connect_info.will_retain = (variable_header->flags & MQTT_CONNECT_FLAG_WILL_RETAIN) != 0;
// 	clientcon->connect_info.will_qos = (variable_header->flags & 0x18) >> 3;

// 	if (!(variable_header->flags & MQTT_CONNECT_FLAG_WILL))
// 	{
// 		// Must be all 0 if no lwt is given
// 		if (clientcon->connect_info.will_retain || clientcon->connect_info.will_qos)
// 		{
// 			MQTT_WARNING("MQTT: Last Will flags invalid\r\n");
// 			MQTT_server_disconnectClientCon(clientcon);
// 			return CONNECTION_REFUSE_SERVER_UNAVAILABLE;
// 		}
// 	}
// 	else
// 	{
// 		uint16_t lw_topic_len = clientcon->mqtt_state.message_length - (2 + msg_used_len);
// 		const char *lw_topic =
// 			mqtt_get_str(&clientcon->mqtt_state.in_buffer[2 + msg_used_len], &lw_topic_len);

// 		if (lw_topic == NULL)
// 		{
// 			MQTT_WARNING("MQTT: Last Will topic invalid\r\n");
// 			MQTT_server_disconnectClientCon(clientcon);
// 			return CONNECTION_REFUSE_SERVER_UNAVAILABLE;
// 		}

// 		clientcon->connect_info.will_topic = (char *)os_zalloc(lw_topic_len + 1);
// 		if (clientcon->connect_info.will_topic != NULL)
// 		{
// 			os_memcpy(clientcon->connect_info.will_topic, lw_topic, lw_topic_len);
// 			clientcon->connect_info.will_topic[lw_topic_len] = 0;
// 			if (Topics_hasWildcards(clientcon->connect_info.will_topic))
// 			{
// 				MQTT_WARNING("MQTT: Last Will topic has wildcards\r\n");
// 				MQTT_server_disconnectClientCon(clientcon);
// 				return CONNECTION_REFUSE_SERVER_UNAVAILABLE;
// 			}
// 			if (clientcon->connect_info.will_topic[0] == '$')
// 			{
// 				MQTT_WARNING("MQTT: Last Will topic starts with '$'\r\n");
// 				MQTT_server_disconnectClientCon(clientcon);
// 				return CONNECTION_REFUSE_SERVER_UNAVAILABLE;
// 			}
// 			MQTT_INFO("MQTT: LWT topic %s\r\n", clientcon->connect_info.will_topic);
// 		}
// 		else
// 		{
// 			MQTT_ERROR("MQTT: Out of mem\r\n");
// 			clientcon->connState = TCP_DISCONNECTING;
// 			return CONNECTION_REFUSE_SERVER_UNAVAILABLE;
// 		}
// 		msg_used_len += 2 + lw_topic_len;

// 		uint16_t lw_data_len = clientcon->mqtt_state.message_length - (2 + msg_used_len);
// 		const char *lw_data = mqtt_get_str(&clientcon->mqtt_state.in_buffer[2 + msg_used_len],
// 										   &lw_data_len);

// 		if (lw_data == NULL)
// 		{
// 			MQTT_WARNING("MQTT: Last Will data invalid\r\n");
// 			MQTT_server_disconnectClientCon(clientcon);
// 			return CONNECTION_REFUSE_SERVER_UNAVAILABLE;
// 		}

// 		clientcon->connect_info.will_data = (char *)os_zalloc(lw_data_len);
// 		clientcon->connect_info.will_data_len = lw_data_len;
// 		if (clientcon->connect_info.will_data != NULL)
// 		{
// 			os_memcpy(clientcon->connect_info.will_data, lw_data, lw_data_len);
// 			MQTT_INFO("MQTT: %d bytes of LWT data\r\n", clientcon->connect_info.will_data_len);
// 		}
// 		else
// 		{
// 			MQTT_ERROR("MQTT: Out of mem\r\n");
// 			clientcon->connState = TCP_DISCONNECTING;
// 			return CONNECTION_REFUSE_SERVER_UNAVAILABLE;
// 		}
// 		msg_used_len += 2 + lw_data_len;
// 	}

// 	clientcon->connState = MQTT_DATA;
// 	return CONNECTION_ACCEPTED;
// }

// void MqttSession::recvMqttData(uint8_t msg_type)
// {
// 	switch (msg_type)
// 	{
// 	case MQTT_MSG_TYPE_SUBSCRIBE:
// 		MQTT_ClientCon_recv_MQTT_MSG_TYPE_SUBSCRIBE(ClientCon);
// 		break;

// 	case MQTT_MSG_TYPE_UNSUBSCRIBE:
// 		MQTT_ClientCon_recv_MQTT_MSG_TYPE_UNSUBSCRIBE(ClientCon);

// 		break;

// 	case MQTT_MSG_TYPE_PUBLISH:
// 		MQTT_ClientCon_recv_MQTT_MSG_TYPE_PUBLISH(ClientCon);

// 		break;

// 	case MQTT_MSG_TYPE_PINGREQ:
// 		MQTT_ClientCon_recv_MQTT_MSG_TYPE_PINGREQ(ClientCon);

// 		break;

// 	case MQTT_MSG_TYPE_DISCONNECT:
// 		MQTT_ClientCon_recv_MQTT_MSG_TYPE_DISCONNECT(ClientCon);
// 		break;

// 	default:
// 		// Ignore
// 		break;
// 	}
// }

// void MqttSession::recvMqttSubscribe()
// {
// 	uint16_t msg_id;
// 	uint16_t topic_index = 4;
// 	uint16_t topic_len;
// 	uint8_t *topic_str;
// 	uint8_t topic_buffer[MQTT_BUF_SIZE];
// 	uint8_t num_subs = 0;
// 	uint8_t ret_codes[MAX_SUBS_PER_REQ];

// 	// 2 byte fixed header + 2 byte variable header + 2 byte len + 1 char + 1 QoS

// 	MQTT_INFO("MQTT: Subscribe received, message_len: %d\r\n", clientcon->mqtt_state.message_length);

// 	if (clientcon->mqtt_state.message_length < MINIMUM_SUBSCRIBE_LENGTH)
// 	{
// 		MQTT_ERROR("MQTT: Too short Subscribe message\r\n");
// 		MQTT_server_disconnectClientCon(clientcon);
// 		return;
// 	}

// 	msg_id = mqtt_get_id(clientcon->mqtt_state.in_buffer, clientcon->mqtt_state.in_buffer_length);
// 	MQTT_INFO("MQTT: Message id %d\r\n", msg_id);

// 	while ((topic_index < clientcon->mqtt_state.message_length) && (num_subs < MAX_SUBS_PER_REQ))
// 	{
// 		topic_len = clientcon->mqtt_state.message_length - topic_index;
// 		topic_str = mqtt_get_str(&clientcon->mqtt_state.in_buffer[topic_index], &topic_len);

// 		if (topic_str == NULL)
// 		{
// 			MQTT_WARNING("MQTT: Subscribe topic invalid\r\n");
// 			MQTT_server_disconnectClientCon(clientcon);
// 			return;
// 		}

// 		topic_index += 2 + topic_len;

// 		if (topic_index >= clientcon->mqtt_state.message_length)
// 		{
// 			MQTT_WARNING("MQTT: Subscribe QoS missing\r\n");
// 			MQTT_server_disconnectClientCon(clientcon);
// 			return;
// 		}

// 		uint8_t topic_QoS = clientcon->mqtt_state.in_buffer[topic_index++];

// 		os_memcpy(topic_buffer, topic_str, topic_len);
// 		topic_buffer[topic_len] = 0;
// 		MQTT_INFO("MQTT: Subscribed topic %s QoS %d\r\n", topic_buffer, topic_QoS);

// 		// the return codes, one per topic
// 		// For now we always give back error or QoS = 0 !!
// 		ret_codes[num_subs++] = add_topic(clientcon, topic_buffer, 0) ? 0 : 0x80;
// 		// iterate_topics(print_topic, 0);
// 	}
// 	MQTT_INFO("MQTT: Subscribe successful\r\n");

// 	clientcon->mqtt_state.outbound_message =
// 		mqtt_msg_suback(&clientcon->mqtt_state.mqtt_connection, ret_codes, num_subs, msg_id);
// 	if (QUEUE_Puts(&clientcon->msgQueue, clientcon->mqtt_state.outbound_message->data,
// 				   clientcon->mqtt_state.outbound_message->length) == -1)
// 	{
// 		MQTT_ERROR("MQTT: Queue full\r\n");
// 	}

// 	find_retainedtopic(topic_buffer, publish_retainedtopic, clientcon);
// }

// void MqttSession::recvMqttUnsubscribe()
// {
// 	//uint8_t msg_type;
// 	//uint8_t msg_qos;
// 	uint16_t msg_id;
// 	//enum mqtt_connect_flag msg_conn_ret;
// 	uint16_t topic_index = 4;
// 	uint16_t topic_len;
// 	//uint8_t *topic_str;
// 	uint8_t topic_buffer[MQTT_BUF_SIZE];
// 	//uint16_t data_len;
// 	//uint8_t *data;
// #define MINIMUM_UNSUBSCRIBE_LENGTH 7
// 	MQTT_INFO("MQTT: Unsubscribe received, message_len: %d\r\n", clientcon->mqtt_state.message_length);
// 	// 2 byte fixed header + 2 byte variable header + 2 byte len + 1 char
// 	if (clientcon->mqtt_state.message_length < MINIMUM_UNSUBSCRIBE_LENGTH)
// 	{
// 		MQTT_ERROR("MQTT: Too short Unsubscribe message\r\n");
// 		MQTT_server_disconnectClientCon(clientcon);
// 		return;
// 	}

// 	msg_id = mqtt_get_id(clientcon->mqtt_state.in_buffer, clientcon->mqtt_state.in_buffer_length);
// 	MQTT_INFO("MQTT: Message id %d\r\n", msg_id);

// 	while (topic_index < clientcon->mqtt_state.message_length)
// 	{
// 		topic_len = clientcon->mqtt_state.message_length - topic_index;
// 		char *topic_str = mqtt_get_str(&clientcon->mqtt_state.in_buffer[topic_index], &topic_len);
// 		if (topic_str == NULL)
// 		{
// 			MQTT_WARNING("MQTT: Subscribe topic invalid\r\n");
// 			MQTT_server_disconnectClientCon(clientcon);
// 			return;
// 		}
// 		topic_index += 2 + topic_len;

// 		os_memcpy(topic_buffer, topic_str, topic_len);
// 		topic_buffer[topic_len] = 0;
// 		MQTT_INFO("MQTT: Unsubscribed topic %s\r\n", topic_buffer);

// 		delete_topic(clientcon, topic_buffer);
// 		// iterate_topics(print_topic, 0);
// 	}
// 	MQTT_INFO("MQTT: Unubscribe successful\r\n");

// 	clientcon->mqtt_state.outbound_message = mqtt_msg_unsuback(&clientcon->mqtt_state.mqtt_connection, msg_id);
// 	if (QUEUE_Puts(&clientcon->msgQueue, clientcon->mqtt_state.outbound_message->data,
// 				   clientcon->mqtt_state.outbound_message->length) == -1)
// 	{
// 		MQTT_ERROR("MQTT: Queue full\r\n");
// 	}
// }

// void MqttSession::recvMqttPublish()
// {
// 	//uint8_t msg_type;
// 	//uint8_t msg_qos;
// 	//uint16_t msg_id;
// 	//enum mqtt_connect_flag msg_conn_ret;
// 	//uint16_t topic_index;
// 	uint16_t topic_len;
// 	uint8_t *topic_str;
// 	uint8_t topic_buffer[MQTT_BUF_SIZE];
// 	uint16_t data_len;
// 	uint8_t *data;
// 	MQTT_INFO("MQTT: Publish received, message_len: %d\r\n", clientcon->mqtt_state.message_length);

// 	topic_len = clientcon->mqtt_state.in_buffer_length;
// 	topic_str = (uint8_t *)mqtt_get_publish_topic(clientcon->mqtt_state.in_buffer, &topic_len);
// 	os_memcpy(topic_buffer, topic_str, topic_len);
// 	topic_buffer[topic_len] = 0;
// 	data_len = clientcon->mqtt_state.in_buffer_length;
// 	data = (uint8_t *)mqtt_get_publish_data(clientcon->mqtt_state.in_buffer, &data_len);

// 	if (topic_buffer[0] == '$')
// 	{
// 		MQTT_WARNING("MQTT: Topic starts with '$'\r\n");
// 	}
// 	else
// 	{
// 		MQTT_INFO("MQTT: Published topic \"%s\"\r\n", topic_buffer);
// 		MQTT_INFO("MQTT: Matches to:\r\n");

// 		// Now find, if anything matches and enque publish message
// 		find_topic(topic_buffer, publish_topic, data, data_len);

// 		if (mqtt_get_retain(clientcon->mqtt_state.in_buffer))
// 		{
// 			update_retainedtopic(topic_buffer, data, data_len, mqtt_get_qos(clientcon->mqtt_state.in_buffer));
// 		}
// 	}
// }

// void MqttSession::recvMqttPingReq()
// {
// 	MQTT_INFO("MQTT: receive MQTT_MSG_TYPE_PINGREQ\r\n");
// 	clientcon->mqtt_state.outbound_message = mqtt_msg_pingresp(&clientcon->mqtt_state.mqtt_connection);
// 	if (QUEUE_Puts(&clientcon->msgQueue, clientcon->mqtt_state.outbound_message->data,
// 				   clientcon->mqtt_state.outbound_message->length) == -1)
// 	{
// 		MQTT_ERROR("MQTT: Queue full\r\n");
// 	}
// }

// void MqttSession::recvMqttDisconnect()
// {
// 	MQTT_INFO("MQTT: receive MQTT_MSG_TYPE_DISCONNECT\r\n");

// 	if (clientcon->connect_info.will_topic != NULL)
// 	{
// 		os_free(clientcon->connect_info.will_topic);
// 		clientcon->connect_info.will_topic = NULL;
// 	}
// 	MQTT_server_disconnectClientCon(clientcon);
// 	return;
// }

// void MqttSession::print_topic(topic_entry *topic) const
// {
// 	MQTT_INFO( "MQTT: Client: %s Topic: \"%s\" QoS: %d\r\n", 
// 	           topic->clientcon->connect_info.client_id, 
// 			   topic->topic,
// 			   topic->qos );
// 	return;
// }

// bool MqttSession::publish_topic(topic_entry *topic_e, uint8_t *topic, uint8_t *data, uint16_t data_len) const
// {
// 	MQTT_ClientCon *clientcon = topic_e->clientcon;
// 	uint16_t message_id = 0;

// 	MQTT_INFO( "MQTT: Client: %s Topic: \"%s\" QoS: %d\r\n", 
// 	           clientcon->connect_info.client_id, 
// 	           topic_e->topic,
// 			   topic_e->qos );

// 	clientcon->mqtt_state.outbound_message = mqtt_msg_publish( &clientcon->mqtt_state.mqtt_connection, 
// 	                                                            topic, 
// 																data, 
// 																data_len, 
// 																topic_e->qos, 
// 																0, 
// 															   &message_id );

// 	if ( QUEUE_Puts( &clientcon->msgQueue, 
// 	                  clientcon->mqtt_state.outbound_message->data,
// 				      clientcon->mqtt_state.outbound_message->length ) == -1 )
// 	{
// 		MQTT_ERROR("MQTT: Queue full\r\n");
// 		return false;
// 	}
// 	return true;
// }

// bool publish_retainedtopic(retained_entry *entry, void *user_data)
// {
// 	MQTT_ClientCon *clientcon = (MQTT_ClientCon *)user_data;
// 	uint16_t message_id = 0;

// 	MQTT_INFO( "MQTT: Client: %s Topic: \"%s\" QoS: %d\r\n", 
// 	           clientcon->connect_info.client_id, 
// 			   entry->topic,
// 			   entry->qos );

// 	clientcon->mqtt_state.outbound_message = mqtt_msg_publish( &clientcon->mqtt_state.mqtt_connection, 
// 	                                                            entry->topic, entry->data, 
// 																entry->data_len, entry->qos,
// 						                                        1, 
// 															   &message_id ); // RJG is message_id needed?

// 	if ( QUEUE_Puts( &clientcon->msgQueue, 
// 	                  clientcon->mqtt_state.outbound_message->data,
// 				      clientcon->mqtt_state.outbound_message->length ) == -1 )
// 	{
// 		MQTT_ERROR("MQTT: Queue full\r\n");
// 		return false;
// 	}
// 	return true;
// }

// bool ICACHE_FLASH_ATTR activate_next_client()
// {
// 	for (MQTT_ClientCon *clientcon = clientcon_list; clientcon != NULL; clientcon = clientcon->next)
// 	{
// 		if ((!QUEUE_IsEmpty(&clientcon->msgQueue)) && clientcon->pCon->state != ESPCONN_CLOSE)
// 		{
// 			MQTT_INFO("MQTT: Next message to client: %s\r\n", clientcon->connect_info.client_id);
// 			system_os_post(MQTT_SERVER_TASK_PRIO, 0, (os_param_t)clientcon);
// 			return true;
// 		}
// 	}
// 	return true; // RJG Should this be false?
// }

// void ICACHE_FLASH_ATTR MQTT_server_disconnectClientCon(MQTT_ClientCon *mqttClientCon)
// {
// 	MQTT_INFO("MQTT: ServerDisconnect\r\n");

// 	disconnecting_clientcon.pCon = mqttClientCon->pCon;
// 	disconnecting_clientcon.pCon->reverse = &disconnecting_clientcon;
// 	MQTT_server_deleteClientCon(mqttClientCon);
// 	system_os_post(MQTT_SERVER_TASK_PRIO, 0, (os_param_t)&disconnecting_clientcon);
// }

// void ICACHE_FLASH_ATTR mqtt_server_timer(void *arg)
// {
// 	MQTT_ClientCon *clientcon = (MQTT_ClientCon *)arg;

// 	if (clientcon->sendTimeout > 0)
// 		clientcon->sendTimeout--;

// 	if (clientcon->connectionTimeout > 0)
// 	{
// 		clientcon->connectionTimeout--;
// 	}
// 	else
// 	{
// 		MQTT_WARNING("MQTT: Connection timeout %ds\r\n", 2 * clientcon->connect_info.keepalive + 10);
// 		MQTT_server_disconnectClientCon(clientcon);
// 	}
// }

// bool ICACHE_FLASH_ATTR delete_client_by_id(const uint8_t *id)
// {
// 	MQTT_ClientCon *clientcon = clientcon_list;

// 	for (clientcon = clientcon_list; clientcon != NULL; clientcon = clientcon->next)
// 	{
// 		if (os_strcmp(id, clientcon->connect_info.client_id) == 0)
// 		{
// 			MQTT_INFO("MQTT: Disconnect client: %s\r\n", clientcon->connect_info.client_id);
// 			MQTT_server_disconnectClientCon(clientcon);
// 			return true;
// 		}
// 	}
// 	return true;
// }

// void ICACHE_FLASH_ATTR MQTT_ServerTask(os_event_t *e)
// {
// 	MQTT_ClientCon *clientcon = (MQTT_ClientCon *)e->par;
// 	uint8_t dataBuffer[MQTT_BUF_SIZE];
// 	uint16_t dataLen;
// 	if (e->par == 0)
// 		return;

// 	MQTT_INFO("MQTT: Server task activated - state %d\r\n", clientcon->connState);

// 	switch (clientcon->connState)
// 	{

// 	case TCP_DISCONNECT:
// 		MQTT_INFO("MQTT: Disconnect\r\n");

// 		if (clientcon->pCon != NULL)
// 			espconn_disconnect(clientcon->pCon);

// 		break;

// 	case TCP_DISCONNECTING:
// 	case MQTT_DATA:
// 		if (!QUEUE_IsEmpty(&clientcon->msgQueue) && clientcon->sendTimeout == 0 &&
// 			QUEUE_Gets(&clientcon->msgQueue, dataBuffer, &dataLen, MQTT_BUF_SIZE) == 0)
// 		{

// 			clientcon->mqtt_state.pending_msg_type = mqtt_get_type(dataBuffer);
// 			clientcon->mqtt_state.pending_msg_id = mqtt_get_id(dataBuffer, dataLen);

// 			clientcon->sendTimeout = MQTT_SEND_TIMOUT;
// 			MQTT_INFO("MQTT: Sending, type: %d, id: %04X\r\n", clientcon->mqtt_state.pending_msg_type,
// 					  clientcon->mqtt_state.pending_msg_id);
// 			espconn_send(clientcon->pCon, dataBuffer, dataLen);

// 			clientcon->mqtt_state.outbound_message = NULL;
// 			break;
// 		}
// 		else
// 		{
// 			if (clientcon->connState == TCP_DISCONNECTING)
// 			{
// 				MQTT_server_disconnectClientCon(clientcon);
// 			}
// 		}
// 		break;
// 	}
// }

// bool ICACHE_FLASH_ATTR MQTT_local_publish(uint8_t *topic, uint8_t *data, uint16_t data_length, uint8_t qos,
// 										  uint8_t retain)
// {
// 	find_topic(topic, publish_topic, data, data_length);
// 	if (retain)
// 		update_retainedtopic(topic, data, data_length, qos);
// 	activate_next_client();
// 	return true;
// }

// bool ICACHE_FLASH_ATTR MQTT_local_subscribe(uint8_t *topic, uint8_t qos)
// {
// 	return add_topic(LOCAL_MQTT_CLIENT, topic, 0);
// }

// bool ICACHE_FLASH_ATTR MQTT_local_unsubscribe(uint8_t *topic)
// {
// 	return delete_topic(LOCAL_MQTT_CLIENT, topic);
// }
//  */
// void ICACHE_FLASH_ATTR MQTT_server_onData(MqttDataCallback dataCb)
// {
// 	concrete_data_cb = dataCb;
// }

// void ICACHE_FLASH_ATTR MQTT_server_onConnect(MqttConnectCallback connectCb)
// {
// 	concrete_connect_cb = connectCb;
// }

// void ICACHE_FLASH_ATTR MQTT_server_onDisconnect(MqttDisconnectCallback disconnectCb)
// {
// 	concrete_disconnect_cb = disconnectCb;
// }
