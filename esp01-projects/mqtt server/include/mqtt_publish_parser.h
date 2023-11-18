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
//     // Example PUBLISH message (replace this with the actual received bytes)
//     std::vector<unsigned char> receivedPublishMessage = {0x30, 0x0C, 0x00, 0x03, 't', 'o', 'p', 'i', 'c', 0x01, 0x23, 'p', 'a', 'y', 'l', 'o', 'a', 'd'};

//     MqttPublishParser publishParser(receivedPublishMessage);
//     publishParser.parsePublishMessage();

//     // Display parsed information
//     std::cout << "Topic Name: " << publishParser.getTopicName() << std::endl;
//     std::cout << "Packet Identifier: " << publishParser.getPacketIdentifier() << std::endl;

//     return 0;
// }

#include <vector>
#include <string>

class MqttPublishParser
{
public:
    MqttPublishParser();
    void parseMessage(const std::vector<unsigned char> &message);

private:
    void parseFixedHeader();
    void parseVariableHeader();
    void parsePayload();
    int parseRemainingLength();
    void parseTopicName();
    void parsePacketIdentifier();

private:
    std::vector<unsigned char> publishMessage_;
    int currentIndex_;
    unsigned char fixedHeader_;
    int remainingLength_;
    std::string topicName_;
    int packetIdentifier_;
};
