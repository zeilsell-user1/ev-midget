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

#include "defaults.h"

#define MQTT_MAX_FIXED_HEADER_SIZE 3

class MqttMsg {
  private:
    unsigned short packetId;
    unsigned char data[MAX_MSG_LENGTH];
    unsigned short length;
    bool dup;
    unsigned char qos;
    bool isSent;
    bool ackNeeded;
    bool isAcked;

    bool createConnect();
    bool processConnect();

    unsigned short mqtt_get_id(unsigned char *buffer, unsigned short length);
    unsigned short GetIdFromPublish(unsigned char *buffer, unsigned short length);
    unsigned short GetIdFromGeneric(unsigned char *buffer, unsigned short length);

  public:
    MqttMsg();
    MqttMsg(unsigned char *msg, unsigned short length);


    enum mqtt_message_type
    {
      MQTT_MSG_TYPE_CONNECT     = 1,
      MQTT_MSG_TYPE_CONNACK     = 2,
      MQTT_MSG_TYPE_PUBLISH     = 3,
      MQTT_MSG_TYPE_PUBACK      = 4,
      MQTT_MSG_TYPE_PUBREC      = 5,
      MQTT_MSG_TYPE_PUBREL      = 6,
      MQTT_MSG_TYPE_PUBCOMP     = 7,
      MQTT_MSG_TYPE_SUBSCRIBE   = 8,
      MQTT_MSG_TYPE_SUBACK      = 9,
      MQTT_MSG_TYPE_UNSUBSCRIBE = 10,
      MQTT_MSG_TYPE_UNSUBACK    = 11,
      MQTT_MSG_TYPE_PINGREQ     = 12,
      MQTT_MSG_TYPE_PINGRESP    = 13,
      MQTT_MSG_TYPE_DISCONNECT  = 14
    };

    enum mqtt_connect_return_code
    {
      CONNECTION_ACCEPTED = 0,
      CONNECTION_REFUSE_PROTOCOL,
      CONNECTION_REFUSE_ID_REJECTED,
      CONNECTION_REFUSE_SERVER_UNAVAILABLE,
      CONNECTION_REFUSE_BAD_USERNAME,
      CONNECTION_REFUSE_NOT_AUTHORIZED
    };

    typedef struct mqtt_connection
    {
      mqtt_message_t message;
      unsigned short message_id;
      unsigned char *buffer;
      unsigned short buffer_length;
    } mqtt_connection_t;
    
    enum mqtt_connect_flag
    {
      MQTT_CONNECT_FLAG_USERNAME = 1 << 7,
      MQTT_CONNECT_FLAG_PASSWORD = 1 << 6,
      MQTT_CONNECT_FLAG_WILL_RETAIN = 1 << 5,
      MQTT_CONNECT_FLAG_WILL = 1 << 2,
      MQTT_CONNECT_FLAG_CLEAN_SESSION = 1 << 1
    };

    struct __attribute((__packed__)) mqtt_connect_variable_header
    {
      unsigned char lengthMsb;
      unsigned char lengthLsb;
      unsigned char magic[4];
      unsigned char version;
      unsigned char flags;
      unsigned char keepaliveMsb;
      unsigned char keepaliveLsb;
    };

    struct __attribute((__packed__)) mqtt_connect_variable_header3
    {
      unsigned char lengthMsb;
      unsigned char lengthLsb;
      unsigned char magic[6];
      unsigned char version;
      unsigned char flags;
      unsigned char keepaliveMsb;
      unsigned char keepaliveLsb;
    };

    struct __attribute((__packed__)) mqtt_connect_variable_header4
    {
      unsigned char lengthMsb;
      unsigned char lengthLsb;
      unsigned char magic[4];
      unsigned char version;
      unsigned char flags;
      unsigned char keepaliveMsb;
      unsigned char keepaliveLsb;
    };

    static inline int mqtt_get_type(unsigned char *buffer) { return (buffer[0] & 0xf0) >> 4; }
    static inline int mqtt_get_connect_return_code(unsigned char *buffer) { return buffer[3]; }
    static inline int mqtt_get_dup(unsigned char *buffer) { return (buffer[0] & 0x08) >> 3; }
    static inline int mqtt_get_qos(unsigned char *buffer) { return (buffer[0] & 0x06) >> 1; }
    static inline int mqtt_get_retain(unsigned char *buffer) { return (buffer[0] & 0x01); }

    mqtt_message_t *mqtt_msg_connect(mqtt_connection_t *connection, mqtt_connection_info_t *info);
    mqtt_message_t *mqtt_msg_connack(mqtt_connection_t *connection, enum mqtt_connect_return_code retcode);
    mqtt_message_t *mqtt_msg_publish(mqtt_connection_t *connection, const char *topic, const char *data, int data_length, int qos, int retain, unsigned short *message_id);
    mqtt_message_t *mqtt_msg_puback(mqtt_connection_t *connection, unsigned short message_id);
    mqtt_message_t *mqtt_msg_pubrec(mqtt_connection_t *connection, unsigned short message_id);
    mqtt_message_t *mqtt_msg_pubrel(mqtt_connection_t *connection, unsigned short message_id);
    mqtt_message_t *mqtt_msg_pubcomp(mqtt_connection_t *connection, unsigned short message_id);
    mqtt_message_t *mqtt_msg_subscribe(mqtt_connection_t *connection, const char *topic, int qos, unsigned short *message_id);
    mqtt_message_t *mqtt_msg_suback(mqtt_connection_t *connection, unsigned char *ret_codes, unsigned char ret_codes_len, unsigned short message_id);
    mqtt_message_t *mqtt_msg_unsubscribe(mqtt_connection_t *connection, const char *topic, unsigned short *message_id);
    mqtt_message_t *mqtt_msg_unsuback(mqtt_connection_t *connection, unsigned short message_id);
    mqtt_message_t *mqtt_msg_pingreq(mqtt_connection_t *connection);
    mqtt_message_t *mqtt_msg_pingresp(mqtt_connection_t *connection);
    mqtt_message_t *mqtt_msg_disconnect(mqtt_connection_t *connection);

  private:
    int init_message(mqtt_connection_t *connection);
    int append_string(mqtt_connection_t *connection, const char *string, int len);
    unsigned short append_message_id(mqtt_connection_t *connection, unsigned short message_id);
    mqtt_message_t *fail_message(mqtt_connection_t * connection);
    mqtt_message_t *fini_message(mqtt_connection_t * connection, int type, int dup, int qos, int retain);
    
};

#endif /* MQTT_MSG_H */
