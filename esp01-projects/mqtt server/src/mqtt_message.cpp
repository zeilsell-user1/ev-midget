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
 *
 *******************************************************************************/

#include <string.h>
#include "mqtt_message.h"

MqttMessage::MqttMessage() {}

void MqttMessage::createConnect(const std::string &clientId)
{
    // CONNECT Message Type
    this->message.push_back(MQTT_MSG_TYPE_CONNECT);

    // Remaining Length (variable header + payload)
    this->message.push_back(12); // Remaining Length

    // Variable Header
    this->message.push_back(0);   // Protocol Name Length MSB
    this->message.push_back(4);   // Protocol Name Length LSB
    this->message.push_back('M'); // Protocol Name: M
    this->message.push_back('Q'); // Protocol Name: Q
    this->message.push_back('T'); // Protocol Name: T
    this->message.push_back('T'); // Protocol Name: T

    this->message.push_back(4); // Protocol Level (MQTT v3.1.1)

    // Connect Flags
    this->message.push_back(0b00000000); // Connect Flags: Clean Session, No Will, No Password, No User Name

    // Keep Alive (two-byte value in seconds)
    this->message.push_back(0);  // Keep Alive MSB
    this->message.push_back(30); // Keep Alive LSB (30 seconds)

    // Payload
    this->message.push_back(0);                 // Client ID Length MSB
    this->message.push_back(clientId.length()); // Client ID Length LSB

    for (char c : clientId)
    {
        message.push_back(static_cast<unsigned char>(c)); // Client ID
    }
}

void MqttMessage::createMqttConnackMessage(bool sessionPresent, MqttConnectReturnCode returnCode)
{
    this->message.push_back(MQTT_MSG_TYPE_CONNACK);
    this->message.push_back(2); // Remaining Length
    this->message.push_back(sessionPresent ? 1 : 0); // Session Present
    this->message.push_back(returnCode);             // Connect Return Code
}

void MqttMessage::createMqttPublishMessage(const std::string &topic, const std::string &payload, bool retain, unsigned char qos)
{
    this->qos = qos;
    this->retain = retain;
    this->message.push_back(MQTT_MSG_TYPE_PUBLISH | (retain ? 0x01 : 0x00) | ((qos & 0x03) << 1)); // PUBLISH | Retain | QoS

    // Remaining Length (variable header + payload)
    int remainingLength = topic.length() + payload.length() + (qos > 0 ? 2 : 0);
    do
    {
        unsigned char digit = remainingLength % 128;
        remainingLength /= 128;
        if (remainingLength > 0)
        {
            digit |= 0x80;
        }
        this->message.push_back(digit);
    } while (remainingLength > 0);

    // Variable Header
    for (char c : topic)
    {
        this->message.push_back(static_cast<unsigned char>(c)); // Topic
    }
    if (qos > 0)
    {
        // Packet Identifier (only for QoS > 0)
        this->message.push_back(0x00); // MSB (Not used in this example)
        this->message.push_back(0x01); // LSB (Not used in this example)
    }

    // Payload
    for (char c : payload)
    {
        this->message.push_back(static_cast<unsigned char>(c)); // Payload
    }
}

void MqttMessage::createMqttPubackMessage(unsigned short packetIdentifier)
{
    this->message.push_back(MQTT_MSG_TYPE_PUBACK);

    // Remaining Length (variable header)
    this->message.push_back(2); // Remaining Length
    this->message.push_back((packetIdentifier >> 8) & 0xFF); // Packet Identifier MSB
    this->message.push_back(packetIdentifier & 0xFF);        // Packet Identifier LSB
}

void MqttMessage::createMqttPubrecMessage(unsigned short packetIdentifier)
{
    this->message.push_back(MQTT_MSG_TYPE_PUBREC);

    // Remaining Length (variable header)
    this->message.push_back(2); // Remaining Length
    this->message.push_back((packetIdentifier >> 8) & 0xFF); // Packet Identifier MSB
    this->message.push_back(packetIdentifier & 0xFF);        // Packet Identifier LSB
}

void MqttMessage::createMqttPubrelMessage(unsigned short packetIdentifier)
{
    this->message.push_back(MQTT_MSG_TYPE_PUBREL | 0x02); // lower nibble must be 0x2
    this->message.push_back(4); // Remaining Length
    this->message.push_back((packetIdentifier >> 8) & 0xFF); // Packet Identifier MSB
    this->message.push_back(packetIdentifier & 0xFF);        // Packet Identifier LSB
    this->message.push_back(0x00);                           // reason code
    this->message.push_back(0x00);                           // property length
}

void MqttMessage::createMqttPubcompMessage(unsigned short packetIdentifier)
{
    this->message.push_back(MQTT_MSG_TYPE_PUBCOMP); // PUBCOMP
    this->message.push_back(2); // Remaining Length
    this->message.push_back((packetIdentifier >> 8) & 0xFF); // Packet Identifier MSB
    this->message.push_back(packetIdentifier & 0xFF);        // Packet Identifier LSB
}

void MqttMessage::createMqttSubscribeMessage(const std::vector<std::string> &topics)
{
    this->message.push_back(MQTT_MSG_TYPE_SUBSCRIBE | 0x02); // SUBSCRIBE | 0b10000010

    // Remaining Length (variable header + payload)
    int remainingLength = 2; // Packet Identifier MSB + Packet Identifier LSB
    int topicsToAdd = std::min(static_cast<int>(topics.size()), MAX_TOPICS_IN_SUBSCRIBE);

    for (int i = 0; i < topicsToAdd; ++i)
    {
        const std::string &topic = topics[i];

        if (topic.length() > MAX_TOPIC_LENGTH)
        {
            // Skip topics that exceed the maximum length
            continue;
        }

        remainingLength += 2 + topic.length() + 1; // Topic Length MSB + Topic Length LSB + Topic + Requested QoS
    }

    do
    {
        unsigned char digit = remainingLength % 128;
        remainingLength /= 128;
        if (remainingLength > 0)
        {
            digit |= 0x80;
        }
        this->message.push_back(digit);
    } while (remainingLength > 0);

    // Variable Header
    this->message.push_back(0x00); // Packet Identifier MSB (Not used in this example)
    this->message.push_back(0x01); // Packet Identifier LSB (Not used in this example)

    // Payload
    for (int i = 0; i < topicsToAdd; ++i)
    {
        const std::string &topic = topics[i];

        if (topic.length() > MAX_TOPIC_LENGTH)
        {
            // Skip topics that exceed the maximum length
            continue;
        }

        this->message.push_back(0);                                          // Topic Length MSB
        this->message.push_back(static_cast<unsigned char>(topic.length())); // Topic Length LSB
        for (char c : topic)
        {
            this->message.push_back(static_cast<unsigned char>(c)); // Topic
        }
        this->message.push_back(0x00); // Requested QoS (0)
    }
}

void MqttMessage::reateMqttSubackMessage(unsigned short packetIdentifier, const std::vector<unsigned char> &grantedQoS)
{
    this->message.push_back(MQTT_MSG_TYPE_SUBACK);
    this->message.push_back(2 + grantedQoS.size()); // Remaining Length
    this->message.push_back((packetIdentifier >> 8) & 0xFF); // Packet Identifier MSB
    this->message.push_back(packetIdentifier & 0xFF);        // Packet Identifier LSB

    // Payload
    for (unsigned char qos : grantedQoS)
    {
        this->message.push_back(qos); // Granted QoS
    }
}

void MqttMessage::createMqttUnsubscribeMessage(const std::vector<std::string> &topics)
{
    this->message.push_back(MQTT_MSG_TYPE_UNSUBSCRIBE | 0x02); // UNSUBSCRIBE | 0b10100010

    // Remaining Length (variable header + payload)
    int remainingLength = 2; // Packet Identifier MSB + Packet Identifier LSB
    int topicsToRemove = std::min(static_cast<int>(topics.size()), MAX_TOPICS_IN_SUBSCRIBE);

    for (int i = 0; i < topicsToRemove; ++i)
    {
        const std::string &topic = topics[i];

        if (topic.length() > MAX_TOPIC_LENGTH)
        {
            // Skip topics that exceed the maximum length
            continue;
        }

        remainingLength += 2 + topic.length(); // Topic Length MSB + Topic Length LSB + Topic
    }

    do
    {
        unsigned char digit = remainingLength % 128;
        remainingLength /= 128;
        if (remainingLength > 0)
        {
            digit |= 0x80;
        }
        this->message.push_back(digit);
    } while (remainingLength > 0);

    // Variable Header
    this->message.push_back(0x00); // Packet Identifier MSB (Not used in this example)
    this->message.push_back(0x01); // Packet Identifier LSB (Not used in this example)

    // Payload
    for (int i = 0; i < topicsToRemove; ++i)
    {
        const std::string &topic = topics[i];

        if (topic.length() > MAX_TOPIC_LENGTH)
        {
            // Skip topics that exceed the maximum length
            continue;
        }

        this->message.push_back(0);                                          // Topic Length MSB
        this->message.push_back(static_cast<unsigned char>(topic.length())); // Topic Length LSB
        for (char c : topic)
        {
            this->message.push_back(static_cast<unsigned char>(c)); // Topic
        }
    }
}

void MqttMessage::createMqttUnsubackMessage(unsigned short packetIdentifier) 
{
    this->message.push_back(MQTT_MSG_TYPE_UNSUBACK);  // UNSUBACK
    this->message.push_back(2);     // Remaining Length
    this->message.push_back((packetIdentifier >> 8) & 0xFF);  // Packet Identifier MSB
    this->message.push_back(packetIdentifier & 0xFF);         // Packet Identifier LSB
}

void MqttMessage::createMqttPingreqMessage() 
{
    this->message.push_back(MQTT_MSG_TYPE_PINGREQ); 
    this->message.push_back(0);     // Remaining Length
}

void MqttMessage::createMqttPingrespMessage() 
{
    this->message.push_back(MQTT_MSG_TYPE_PINGRESP);  
    this->message.push_back(0);     // Remaining Length
}

void MqttMessage::createMqttDisconnectMessage() 
{
    this->message.push_back(MQTT_MSG_TYPE_DISCONNECT); 
    this->message.push_back(0);     // Remaining Length
}
