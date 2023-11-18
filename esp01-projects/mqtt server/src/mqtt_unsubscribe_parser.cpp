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
//     // Example UNSUBSCRIBE message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedUnsubscribeMessage = {
//         0xA2, 0x0F, 0x00, 0x01, 0x00, 0x03, 'f', 'o', 'o', 0x00, 0x03, 'b', 'a', 'r'};

//     MqttUnsubscribeParser unsubscribeParser(receivedUnsubscribeMessage);
//     unsubscribeParser.parseUnsubscribeMessage();

//     // Display parsed information
//     std::cout << "Packet Identifier: " << unsubscribeParser.getPacketIdentifier() << std::endl;

//     return 0;
// }

#include "defaults.h"
#include "mqtt_unsubscribe_parser.h"

MqttUnsubscribeParser::MqttUnsubscribeParser() {}

void MqttUnsubscribeParser::parseMessage(const std::vector<unsigned char> &unsubscribeMessage)
{
    unsubscribeMessage_ = unsubscribeMessage;
    currentIndex_ = 0;
    parseFixedHeader();
    parseVariableHeader();
    parsePayload();
}

void MqttUnsubscribeParser::parseFixedHeader()
{
    // The first byte of the message is the fixed header
    fixedHeader_ = unsubscribeMessage_[currentIndex_++];
}

void MqttUnsubscribeParser::parseVariableHeader()
{
    // The next two bytes represent the remaining length
    remainingLength_ = parseRemainingLength();

    // The next bytes are part of the variable header
    parsePacketIdentifier();
}

int MqttUnsubscribeParser::parseRemainingLength()
{
    int multiplier = 1;
    int value = 0;
    unsigned char byte;

    do
    {
        byte = unsubscribeMessage_[currentIndex_++];
        value += (byte & 127) * multiplier;
        multiplier *= 128;
    } while ((byte & 128) != 0);

    return value;
}

void MqttUnsubscribeParser::parsePacketIdentifier()
{
    packetIdentifier_ = (unsubscribeMessage_[currentIndex_++] << 8) + unsubscribeMessage_[currentIndex_++];
}

void MqttUnsubscribeParser::parsePayload()
{
    // Parse each unsubscribe topic
    while (currentIndex_ < unsubscribeMessage_.size())
    {
        // Parse the topic name
        std::string topicName = parseString();

        // Display parsed topic information
        MQTT_INFO("Unsubscribe: Topic=%s", topicName);
    }
}

std::string MqttUnsubscribeParser::parseString()
{
    int stringLength = (unsubscribeMessage_[currentIndex_++] << 8) + unsubscribeMessage_[currentIndex_++];
    std::string str(unsubscribeMessage_.begin() + currentIndex_, unsubscribeMessage_.begin() + currentIndex_ + stringLength);
    currentIndex_ += stringLength;
    return str;
}

int MqttUnsubscribeParser::getPacketIdentifier() const
{
    return packetIdentifier_;
}
