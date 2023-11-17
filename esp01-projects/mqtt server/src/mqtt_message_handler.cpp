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
//     // Example data (replace this with the actual data received)
//     unsigned char data[] = {0x10, 0x1F, 'M', 'Q', 'T', 'T', 0x04, 0x04, 0xCE, 0x00, 0x3C, 'C', 'l', 'i', 'e', 'n', 't', 'I', 'D'};
//     unsigned short data_len = sizeof(data) / sizeof(data[0]);

//     // Handle the MQTT message
//     MqttMessageHandler::handleMessage(data, data_len);

//     return 0;
//}

#include <vector>
#include "defaults.h"
#include "mqtt_message_handler.h"

void MqttMessageHandler::handleMessage(const unsigned char *data, unsigned short data_len)
{
    // Convert data to vector for ease of use
    std::vector<unsigned char> message(data, data + data_len);

    // Check if the message is long enough to contain the fixed header
    if (message.size() < 2)
    {
        MQTT_ERROR("Error: Invalid MQTT message format");
        return;
    }

    // Determine the message type based on the fixed header
    unsigned char messageType = message[0] >> 4;

    // Select the appropriate parser based on the message type
    switch (messageType)
    {
    case 1:
    {
        //MqttConnectParser connectParser;
        //connectParser.parseMessage(message);
        break;
    }
    case 2:
    {
        //MqttConnackParser connackParser;
        //connackParser.parseMessage(message);
        break;
    }
    // Add cases for other message types (PUBLISH, PUBACK, etc.)
    default:
        MQTT_ERROR("Error: Unknown MQTT message type %d", messageType);
    }
}
