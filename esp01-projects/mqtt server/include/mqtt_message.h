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

  std::vector<unsigned char> message;
  unsigned char qos;
  bool retain;
};

#endif /* MQTT_MSG_H */

// struct MqttConnection
// {
//   unsigned short message_id;
//   unsigned char *buffer;
//   unsigned short buffer_length;
// };

// enum MqttConnectionFlag
// {
//   MQTT_CONNECT_FLAG_USERNAME = 1 << 7,
//   MQTT_CONNECT_FLAG_PASSWORD = 1 << 6,
//   MQTT_CONNECT_FLAG_WILL_RETAIN = 1 << 5,
//   MQTT_CONNECT_FLAG_WILL = 1 << 2,
//   MQTT_CONNECT_FLAG_CLEAN_SESSION = 1 << 1
// };

// struct __attribute((__packed__)) MqttConnectionVariableheader
// {
//   unsigned char lengthMsb;
//   unsigned char lengthLsb;
//   unsigned char magic[4];
//   unsigned char version;
//   unsigned char flags;
//   unsigned char keepaliveMsb;
//   unsigned char keepaliveLsb;
// };

// struct __attribute((__packed__)) MqttConnectionVariableheader3
// {
//   unsigned char lengthMsb;
//   unsigned char lengthLsb;
//   unsigned char magic[6];
//   unsigned char version;
//   unsigned char flags;
//   unsigned char keepaliveMsb;
//   unsigned char keepaliveLsb;
// };

// struct __attribute((__packed__)) MqttConnectionVariableheader4
// {
//   unsigned char lengthMsb;
//   unsigned char lengthLsb;
//   unsigned char magic[4];
//   unsigned char version;
//   unsigned char flags;
//   unsigned char keepaliveMsb;
//   unsigned char keepaliveLsb;
// };

// static inline int mqttGetType(unsigned char *buffer) { return (buffer[0] & 0xf0) >> 4; }
// static inline int mqttGetConnectionReturnCode(unsigned char *buffer) { return buffer[3]; }
// static inline int mqttGetDup(unsigned char *buffer) { return (buffer[0] & 0x08) >> 3; }
// static inline int mqttGetQos(unsigned char *buffer) { return (buffer[0] & 0x06) >> 1; }
// static inline int mqttGetRetain(unsigned char *buffer) { return (buffer[0] & 0x01); }

// unsigned short packetId;
// MqttMessageType type;
// unsigned char data[MAX_MSG_LENGTH];
// unsigned short length;
// bool dup;
// bool isSent;
// bool ackNeeded;
// bool isAcked;

// void connect();

// ///////////////////////////////////////////////////////////

// bool createConnect();
// bool processConnect();

// unsigned short mqttGetId(unsigned char *buffer, unsigned short length);
// unsigned short GetIdFromPublish(unsigned char *buffer, unsigned short length);
// unsigned short GetIdFromGeneric(unsigned char *buffer, unsigned short length);

// int appendString(MqttConnection *connection, const char *string, int len);
// unsigned short appendMessageId(MqttConnection *connection, unsigned short message_id);

// mqtt_message_t *mqtt_msg_connect(mqtt_connection_t *connection, mqtt_connection_info_t *info);
// mqtt_message_t *mqtt_msg_connack(mqtt_connection_t *connection, enum mqtt_connect_return_code retcode);
// mqtt_message_t *mqtt_msg_publish(mqtt_connection_t *connection, const char *topic, const char *data, int data_length, int qos, int retain, unsigned short *message_id);
// mqtt_message_t *mqtt_msg_puback(mqtt_connection_t *connection, unsigned short message_id);
// mqtt_message_t *mqtt_msg_pubrec(mqtt_connection_t *connection, unsigned short message_id);
// mqtt_message_t *mqtt_msg_pubrel(mqtt_connection_t *connection, unsigned short message_id);
// mqtt_message_t *mqtt_msg_pubcomp(mqtt_connection_t *connection, unsigned short message_id);
// mqtt_message_t *mqtt_msg_subscribe(mqtt_connection_t *connection, const char *topic, int qos, unsigned short *message_id);
// mqtt_message_t *mqtt_msg_suback(mqtt_connection_t *connection, unsigned char *ret_codes, unsigned char ret_codes_len, unsigned short message_id);
// mqtt_message_t *mqtt_msg_unsubscribe(mqtt_connection_t *connection, const char *topic, unsigned short *message_id);
// mqtt_message_t *mqtt_msg_unsuback(mqtt_connection_t *connection, unsigned short message_id);
// mqtt_message_t *mqtt_msg_pingreq(mqtt_connection_t *connection);
// mqtt_message_t *mqtt_msg_pingresp(mqtt_connection_t *connection);
// mqtt_message_t *mqtt_msg_disconnect(mqtt_connection_t *connection);
// mqtt_message_t *fail_message(mqtt_connection_t * connection);
// mqtt_message_t *fini_message(mqtt_connection_t * connection, int type, int dup, int qos, int retain);