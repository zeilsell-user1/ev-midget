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

#ifndef MQTT_MSG_H
#define MQTT_MSG_H

#include <vector>
#include "defaults.h"
#include <string>

#define MQTT_MAX_FIXED_HEADER_SIZE 3

class MqttMessage
{
public:
  enum MqttConnectReturnCode
  {
    CONNECTION_ACCEPTED = 0,
    CONNECTION_REFUSE_PROTOCOL,
    CONNECTION_REFUSE_ID_REJECTED,
    CONNECTION_REFUSE_SERVER_UNAVAILABLE,
    CONNECTION_REFUSE_BAD_USERNAME,
    CONNECTION_REFUSE_NOT_AUTHORIZED
  };

  MqttMessage();
  void createConnect(const std::string &clientId);
  void createMqttConnackMessage(bool sessionPresent, MqttConnectReturnCode returnCode);
  void createMqttPublishMessage(const std::string &topic, const std::string &payload, bool retain, unsigned char qos);
  void createMqttPubackMessage(unsigned short packetIdentifier);
  void createMqttPubrecMessage(unsigned short packetIdentifier);
  void createMqttPubrelMessage(unsigned short packetIdentifier);
  void createMqttPubcompMessage(unsigned short packetIdentifier);
  void createMqttSubscribeMessage(const std::vector<std::string>& topics);
  void reateMqttSubackMessage(unsigned short packetIdentifier, const std::vector<unsigned char>& grantedQoS);
  void createMqttUnsubscribeMessage(const std::vector<std::string>& topics);
  void createMqttUnsubackMessage(unsigned short packetIdentifier);
  void createMqttPingreqMessage();
  void createMqttPingrespMessage();
  void createMqttDisconnectMessage();

private:
  enum MqttMessageType
  {
    MQTT_MSG_TYPE_CONNECT = 0x10,
    MQTT_MSG_TYPE_CONNACK = 0x20,
    MQTT_MSG_TYPE_PUBLISH = 0x30,
    MQTT_MSG_TYPE_PUBACK = 0x40,
    MQTT_MSG_TYPE_PUBREC = 0x50,
    MQTT_MSG_TYPE_PUBREL = 0x60,
    MQTT_MSG_TYPE_PUBCOMP = 0x70,
    MQTT_MSG_TYPE_SUBSCRIBE = 0x80,
    MQTT_MSG_TYPE_SUBACK = 0x90,
    MQTT_MSG_TYPE_UNSUBSCRIBE = 0xA0,
    MQTT_MSG_TYPE_UNSUBACK = 0xB0,
    MQTT_MSG_TYPE_PINGREQ = 0xC0,
    MQTT_MSG_TYPE_PINGRESP = 0xD0,
    MQTT_MSG_TYPE_DISCONNECT = 0xE0
  };

  std::vector<unsigned char> message_;
  unsigned char qos_;
  bool retain_;
};

#endif /* MQTT_MSG_H */
