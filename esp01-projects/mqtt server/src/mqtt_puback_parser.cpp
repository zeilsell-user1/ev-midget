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
//     // Example PUBACK message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedPubackMessage = {0x40, 0x02, 0x01, 0x23};

//     MqttPubackParser pubackParser(receivedPubackMessage);
//     pubackParser.parsePubackMessage();

//     // Display parsed information
//     std::cout << "Packet Identifier: " << pubackParser.getPacketIdentifier() << std::endl;

//     return 0;
// }

#include "mqtt_puback_parser.h"

MqttPubackParser::MqttPubackParser() {}

void MqttPubackParser::parseMessage(const std::vector<unsigned char> &pubackMessage)
{
    pubackMessage_ = pubackMessage;
    currentIndex_ = 0;
    parseFixedHeader();
    parseVariableHeader();
}

void MqttPubackParser::parseFixedHeader()
{
    // The first byte of the message is the fixed header
    fixedHeader_ = pubackMessage_[currentIndex_++];
}

void MqttPubackParser::parseVariableHeader()
{
    // The next two bytes represent the remaining length
    remainingLength_ = parseRemainingLength();

    // The next bytes are part of the variable header
    parsePacketIdentifier();
}

int MqttPubackParser::parseRemainingLength()
{
    int multiplier = 1;
    int value = 0;
    unsigned char byte;

    do
    {
        byte = pubackMessage_[currentIndex_++];
        value += (byte & 127) * multiplier;
        multiplier *= 128;
    } while ((byte & 128) != 0);

    return value;
}

void MqttPubackParser::parsePacketIdentifier()
{
    packetIdentifier_ = (pubackMessage_[currentIndex_++] << 8) + pubackMessage_[currentIndex_++];
}

int MqttPubackParser::getPacketIdentifier() const
{
    return packetIdentifier_;
}
