
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
//     // Example PUBCOMP message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedPubcompMessage = {0x70, 0x02, 0x01, 0x23};

//     MqttPubcompParser pubcompParser(receivedPubcompMessage);
//     pubcompParser.parsePubcompMessage();

//     // Display parsed information
//     std::cout << "Packet Identifier: " << pubcompParser.getPacketIdentifier() << std::endl;

//     return 0;
// }

#include "mqtt_pubcomp_parser.h"

MqttPubcompParser::MqttPubcompParser() {}

void MqttPubcompParser::parseMessage(const std::vector<unsigned char> &pubcompMessage)
{
    pubcompMessage_ = pubcompMessage;
    currentIndex_ = 0;
    parseFixedHeader();
    parseVariableHeader();
}

void MqttPubcompParser::parseFixedHeader()
{
    // The first byte of the message is the fixed header
    fixedHeader_ = pubcompMessage_[currentIndex_++];
}

void MqttPubcompParser::parseVariableHeader()
{
    // The next two bytes represent the remaining length
    remainingLength_ = parseRemainingLength();

    // The next bytes are part of the variable header
    parsePacketIdentifier();
}

int MqttPubcompParser::parseRemainingLength()
{
    int multiplier = 1;
    int value = 0;
    unsigned char byte;

    do
    {
        byte = pubcompMessage_[currentIndex_++];
        value += (byte & 127) * multiplier;
        multiplier *= 128;
    } while ((byte & 128) != 0);

    return value;
}

void MqttPubcompParser::parsePacketIdentifier()
{
    packetIdentifier_ = (pubcompMessage_[currentIndex_++] << 8) + pubcompMessage_[currentIndex_++];
}

int MqttPubcompParser::getPacketIdentifier() const
{
    return packetIdentifier_;
}
