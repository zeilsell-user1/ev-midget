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

#include <vector>
#include "mqtt_message_parser.h"

class MqttConnackParser : public MqttMessageParser
{
public:
    MqttConnackParser();
    void parseMessage(const std::vector<unsigned char> &connackMessage);

private:
    void parseFixedHeader();
    void parseVariableHeader();
    int parseRemainingLength();
    void parseSessionPresent();
    void parseConnackReturnCode();

private:
    enum class MqttConnackReturnCode : unsigned char
    {
        ConnectionAccepted = 0x00,
        ConnectionRefusedUnacceptableProtocolVersion = 0x01,
        ConnectionRefusedIdentifierRejected = 0x02,
        ConnectionRefusedServerUnavailable = 0x03,
        ConnectionRefusedBadUsernameOrPassword = 0x04,
        ConnectionRefusedNotAuthorized = 0x05,
    };

private:
    std::vector<unsigned char> connackMessage_;
    int currentIndex_;
    unsigned char fixedHeader_;
    int remainingLength_;
    bool sessionPresent_;
    MqttConnackReturnCode connackReturnCode_;
};
