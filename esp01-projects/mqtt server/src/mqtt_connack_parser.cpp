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
//     // Example CONNACK message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedConnackMessage = {0x20, 0x02, 0x01, 0x00};

//     MqttConnackParser connackParser(receivedConnackMessage);
//     connackParser.parseConnackMessage();

//     // Display parsed information
//     std::cout << "Session Present: " << (connackParser.getSessionPresent() ? "true" : "false") << std::endl;
//     std::cout << "CONNACK Return Code: " << static_cast<int>(connackParser.getConnackReturnCode()) << std::endl;

//     return 0;
// }

#include "mqtt_connack_parser.h"

MqttConnackParser::MqttConnackParser() {}

void MqttConnackParser::parseMessage(const std::vector<unsigned char> &connackMessage)
{
    connackMessage_ = connackMessage;
    currentIndex_ = 0;
    parseFixedHeader();
    parseVariableHeader();
}

void MqttConnackParser::parseFixedHeader()
{
    // The first byte of the message is the fixed header
    fixedHeader_ = connackMessage_[currentIndex_++];
}

void MqttConnackParser::parseVariableHeader()
{
    // The next two bytes represent the remaining length
    remainingLength_ = parseRemainingLength();

    // The next bytes are part of the variable header
    parseSessionPresent();
    parseConnackReturnCode();
}

int MqttConnackParser::parseRemainingLength()
{
    int multiplier = 1;
    int value = 0;
    unsigned char byte;

    do
    {
        byte = connackMessage_[currentIndex_++];
        value += (byte & 127) * multiplier;
        multiplier *= 128;
    } while ((byte & 128) != 0);

    return value;
}

void MqttConnackParser::parseSessionPresent()
{
    sessionPresent_ = (connackMessage_[currentIndex_++] & 0x01) != 0;
}

void MqttConnackParser::parseConnackReturnCode()
{
    connackReturnCode_ = static_cast<MqttConnackReturnCode>(connackMessage_[currentIndex_++]);
}
