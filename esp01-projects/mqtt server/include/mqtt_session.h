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
#include "mqtt_message.h"

class MqttSession
{
public:
  MqttSession();
  MqttSession(std::shared_ptr<TcpSession> tcpSession);
  void setSessionFalse();
  bool isSessionValid();
  std::shared_ptr<TcpSession> getTcpSession();

  void handleTcpDisconnect(void *arg);
  void handleTcpMessageSent(void *arg);
  void handleTcpIncomingMessage(void *arg, char *pdata, unsigned short len);

private: // state machine for the MQTT session
  void WaitForConnect_HandleMsg(MqttMessage msg);
  void Connected_HandleMsg(MqttMessage msg);
  void WaitForPubRel_HandleMsg(MqttMessage msg);
  void Disconnected_HandleMsg(MqttMessage msg);

private: // utility methods
  void print_topic(MqttTopic *topic) const;
  bool publish_topic(MqttTopic *topic, unsigned char *data, unsigned short data_len) const;

private:
  bool sessionValid_;
  std::shared_ptr<TcpSession> tcpSession_;
  unsigned char will_qos_;
  int will_retain_;
  int clean_session_;
  unsigned char clientId_[23];
  unsigned char IPAddress_[4];
  unsigned long sessionExpiryIntervalTimeout_;
};

#endif /* MQTT_SESSION_H */