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
//     // Example DISCONNECT message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedDisconnectMessage = {0xE0, 0x00};

//     MqttDisconnectParser disconnectParser(receivedDisconnectMessage);
//     disconnectParser.parseDisconnectMessage();

//     // No specific information to display for DISCONNECT

//     return 0;
// }

#include "mqtt_disconnect_parser.h"

MqttDisconnectParser::MqttDisconnectParser() {}

void MqttDisconnectParser::parseMessage(const std::vector<unsigned char> &disconnectMessage)
{
    disconnectMessage_ = disconnectMessage;
    currentIndex_ = 0;
    parseFixedHeader();
}

void MqttDisconnectParser::parseFixedHeader()
{
    // The first byte of the message is the fixed header
    fixedHeader_ = disconnectMessage_[currentIndex_++];
}
