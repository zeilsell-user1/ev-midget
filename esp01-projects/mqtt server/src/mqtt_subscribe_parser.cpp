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

// int main() {
//     // Example SUBSCRIBE message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedSubscribeMessage = {
//         0x82, 0x0D, 0x00, 0x01, 0x00, 0x03, 'f', 'o', 'o', 0x01, 'b', 'a', 'r', 0x02};

//     MqttSubscribeParser subscribeParser(receivedSubscribeMessage);
//     subscribeParser.parseSubscribeMessage();

//     // Display parsed information
//     std::cout << "Packet Identifier: " << subscribeParser.getPacketIdentifier() << std::endl;

//     return 0;
// }

#include "mqtt_subscribe_parser.h"

MqttSubscribeParser::MqttSubscribeParser() {}

void MqttSubscribeParser::parseMessage(const std::vector<unsigned char> &subscribeMessage)
{
    subscribeMessage_ = subscribeMessage;
    currentIndex_ = 0;
    parseFixedHeader();
    parseVariableHeader();
    parsePayload();
}

void MqttSubscribeParser::parseFixedHeader()
{
    // The first byte of the message is the fixed header
    fixedHeader_ = subscribeMessage_[currentIndex_++];
}

void MqttSubscribeParser::parseVariableHeader()
{
    // The next two bytes represent the remaining length
    remainingLength_ = parseRemainingLength();

    // The next bytes are part of the variable header
    parsePacketIdentifier();
}

int MqttSubscribeParser::parseRemainingLength()
{
    int multiplier = 1;
    int value = 0;
    unsigned char byte;

    do
    {
        byte = subscribeMessage_[currentIndex_++];
        value += (byte & 127) * multiplier;
        multiplier *= 128;
    } while ((byte & 128) != 0);

    return value;
}

void MqttSubscribeParser::parsePacketIdentifier()
{
    packetIdentifier_ = (subscribeMessage_[currentIndex_++] << 8) + subscribeMessage_[currentIndex_++];
}

void MqttSubscribeParser::parsePayload()
{
    // Parse each subscription topic and QoS pair
    while (currentIndex_ < subscribeMessage_.size())
    {
        // Parse the topic name
        std::string topicName = parseString();

        // Parse the QoS level
        unsigned char qosLevel = subscribeMessage_[currentIndex_++];

        // Display parsed subscription information
        std::cout << "Subscription: Topic=" << topicName << ", QoS=" << static_cast<int>(qosLevel) << std::endl;
    }
}

std::string MqttSubscribeParser::parseString()
{
    int stringLength = (subscribeMessage_[currentIndex_++] << 8) + subscribeMessage_[currentIndex_++];
    std::string str(subscribeMessage_.begin() + currentIndex_, subscribeMessage_.begin() + currentIndex_ + stringLength);
    currentIndex_ += stringLength;
    return str;
}

int MqttSubscribeParser::getPacketIdentifier() const
{
    return packetIdentifier_;
}
