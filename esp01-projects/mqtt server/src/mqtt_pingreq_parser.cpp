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
//     // Example PINGREQ message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedPingreqMessage = {0xC0, 0x00};

//     MqttPingreqParser pingreqParser(receivedPingreqMessage);
//     pingreqParser.parsePingreqMessage();

//     // No specific information to display for PINGREQ

//     return 0;
// }

#include <iostream>
#include <vector>
#include "mqtt_pingreq_parser.h"

MqttPingreqParser::MqttPingreqParser() {}

// Parse the PINGREQ message
void MqttPingreqParser::parseMessage(const std::vector<unsigned char> &pingreqMessage)
{
    pingreqMessage_ = pingreqMessage;
    currentIndex_ = 0;
    parseFixedHeader();
}

void MqttPingreqParser::parseFixedHeader()
{
    // The first byte of the message is the fixed header
    fixedHeader_ = pingreqMessage_[currentIndex_++];
}
