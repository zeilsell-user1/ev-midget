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
//     // Example CONNECT message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedConnectMessage = {
//         0x10, 0x1F,  // Fixed Header
//         'M', 'Q', 'T', 'T', 0x04,  // Protocol Name
//         0x04,  // Protocol Level
//         0xCE,  // Connect Flags
//         0x00, 0x3C,  // Keep Alive
//         // Payload (client identifier, will topic, etc.)
//         'C', 'l', 'i', 'e', 'n', 't', 'I', 'D',
//     };

//     MqttConnectParser connectParser(receivedConnectMessage);
//     connectParser.parseConnectMessage();

//     return 0;
// }

#include <string>
#include "mqtt_connect_parser.h"

MqttConnectParser::MqttConnectParser() {}

// Parse the CONNECT message
void MqttConnectParser::parseMessage(const std::vector<unsigned char> &connectMessage)
{
    connectMessage_ = connectMessage;
    currentIndex_ = 0;
    parseFixedHeader();
    parseVariableHeader();
    parsePayload();
}

void MqttConnectParser::parseFixedHeader()
{
    // The first byte of the message is the fixed header
    fixedHeader_ = connectMessage_[currentIndex_++];
}

void MqttConnectParser::parseVariableHeader()
{
    // The next two bytes represent the remaining length
    remainingLength_ = parseRemainingLength();

    // The next bytes are part of the variable header
    parseProtocolName();
    parseProtocolLevel();
    parseConnectFlags();
    parseKeepAlive();
}

void MqttConnectParser::parsePayload()
{
    // The remaining bytes are part of the payload (client identifier, will topic, etc.)
    // Parse as needed based on the CONNECT flags
    // Note: This is a simplified example, and you might need to handle various cases
}

int MqttConnectParser::parseRemainingLength()
{
    int multiplier = 1;
    int value = 0;
    unsigned char byte;

    do
    {
        byte = connectMessage_[currentIndex_++];
        value += (byte & 127) * multiplier;
        multiplier *= 128;
    } while ((byte & 128) != 0);

    return value;
}

void MqttConnectParser::parseProtocolName()
{
    // Assuming a fixed-length protocol name of "MQTT"
    const std::string expectedProtocolName = "MQTT";
    std::string receivedProtocolName(connectMessage_.begin() + currentIndex_,
                                     connectMessage_.begin() + currentIndex_ + expectedProtocolName.length());

    currentIndex_ += expectedProtocolName.length();

    if (receivedProtocolName != expectedProtocolName)
    {
        MQTT_ERROR("Error: Unexpected protocol name");
    }
}

void MqttConnectParser::parseProtocolLevel()
{
    protocolLevel_ = connectMessage_[currentIndex_++];
}

void MqttConnectParser::parseConnectFlags()
{
    connectFlags_ = connectMessage_[currentIndex_++];
}

void MqttConnectParser::parseKeepAlive()
{
    // Assuming a fixed-length 2-byte keep-alive field
    keepAlive_ = (connectMessage_[currentIndex_++] << 8) + connectMessage_[currentIndex_++];
}
