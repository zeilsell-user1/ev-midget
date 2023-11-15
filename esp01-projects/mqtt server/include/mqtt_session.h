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

#ifndef MQTT_SESSION_H
#define MQTT_SESSION_H

#include <memory>

#include "defaults.h"
#include "tcp_session.h"
#include "mqtt_topic.h"
#include "mqtt_msg.h"

class MqttSession
{
public:
    MqttSession();
    MqttSession(std::shared_ptr<TcpSession> tcpSession);
    void setSessionFalse();
    bool isSessionValid();
    std::shared_ptr<TcpSession> getTcpSession();

    void handleTcpConnect(void *args);
    void handleTcpDisconnect(void *args);
    void handleTcpMessageSent(void *args);
    void handleTcpMessageAcknowledged(void *args);
    void handleTcpIncomingMessage(void *arg, char *pdata, unsigned short len);

private:
    bool sessionValid;
    std::shared_ptr<TcpSession> tcpSession;
    unsigned char will_qos;
    int will_retain;
    int clean_session;
    unsigned char clientId[23];
    unsigned char IPAddress[4];
    unsigned long sessionExpiryIntervalTimeout;

  // state machine for the MQTT session
    void WaitForConnect_HandleMsg(MqttMsg msg);
    void Connected_HandleMsg(MqttMsg msg);
    void WaitForPubRel_HandleMsg(MqttMsg msg);
    void Disconnected_HandleMsg(MqttMsg msg);

    void print_topic(MqttTopic *topic) const;
    bool publish_topic(MqttTopic *topic, unsigned char *data, unsigned short data_len) const;

};

#endif /* MQTT_SESSION_H */