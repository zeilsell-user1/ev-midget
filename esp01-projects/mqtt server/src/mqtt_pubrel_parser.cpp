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
//     // Example PUBREL message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedPubrelMessage = {0x62, 0x02, 0x01, 0x23};

//     MqttPubrelParser pubrelParser(receivedPubrelMessage);
//     pubrelParser.parsePubrelMessage();

//     // Display parsed information
//     std::cout << "Packet Identifier: " << pubrelParser.getPacketIdentifier() << std::endl;

//     return 0;
// }

#include "mqtt_pubrel_parser.h"

MqttPubrelParser::MqttPubrelParser() {}

void MqttPubrelParser::parseMessage(const std::vector<unsigned char> &pubrelMessage)
{
    pubrelMessage_ = pubrelMessage;
    currentIndex_ = 0;
    parseFixedHeader();
    parseVariableHeader();
}

void MqttPubrelParser::parseFixedHeader()
{
    // The first byte of the message is the fixed header
    fixedHeader_ = pubrelMessage_[currentIndex_++];
}

void MqttPubrelParser::parseVariableHeader()
{
    // The next two bytes represent the remaining length
    remainingLength_ = parseRemainingLength();

    // The next bytes are part of the variable header
    parsePacketIdentifier();
}

int MqttPubrelParser::parseRemainingLength()
{
    int multiplier = 1;
    int value = 0;
    unsigned char byte;

    do
    {
        byte = pubrelMessage_[currentIndex_++];
        value += (byte & 127) * multiplier;
        multiplier *= 128;
    } while ((byte & 128) != 0);

    return value;
}

void MqttPubrelParser::parsePacketIdentifier()
{
    packetIdentifier_ = (pubrelMessage_[currentIndex_++] << 8) + pubrelMessage_[currentIndex_++];
}

int MqttPubrelParser::getPacketIdentifier() const
{
    return packetIdentifier_;
}
