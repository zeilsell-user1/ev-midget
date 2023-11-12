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

#ifndef _MQTT_SERVER_H_
#define _MQTT_SERVER_H_

#include "tcp_server.h"
#include "tcp_session.h"
#include "mqtt_session_list.h"
#include "mqtt_msg.h"
#include "mqtt_topic.h"

// The MQTT Server can run as either the client or the server. It provides 
// the managment of TCP and MQTT connections, processes the received
// incoming messages and executes the appropriate actions.
class MqttServer
{
  private:
    MqttSessionList sessionList;
    TcpServer tcpServer;
    unsigned char* ipAddress; 
    unsigned short port;

    MqttServer();

    // void print_topic(topic_entry *topic) const;
    // bool publish_topic(topic_entry *topic_e, uint8_t *topic, uint8_t *data, uint16_t data_len) const;

public:
    MqttServer(unsigned char* ipAddress, unsigned short port); // client
    MqttServer(unsigned short portno); // server

    void handleTcpConnect(void *args);
    void handleTcpConnect(void *args);
    void handleTcpDisconnect(void *args);
    void handleTcpMessageSent(void *args);
    void handleTcpMessageAcknowledged(void *args);
    void handleTcpIncomingMessage(void *arg, char *pdata, unsigned short len);

    // state machine for the MQTT session
    void WaitForConnect_HandleMsg(MqttMsg msg);
    void Connected_HandleMsg(MqttMsg msg);
    void WaitForPubRel_HandleMsg(MqttMsg msg);
    void Disconnected_HandleMsg(MqttMsg msg);

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
