/*******************************************************************************
 * Copyright (c) 2023 George Consultant Ltd.
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
#include <cstdio>

#ifndef MQTT_BROKER_PORT
#define MQTT_BROKER_PORT 1883
#endif 

#ifndef MAX_SUBSCRITIONS
#define MAX_SUBSCRITIONS 30
#endif 

#ifndef MAX_RETAINED_TOPICS
#define MAX_RETAINED_TOPICS 30
#endif

#ifndef MAX_TOPIC_LIST_ENTRIES
#define MAX_TOPIC_LIST_ENTRIES 30
#endif

#ifndef MAX_TOPIC_LENGTH // YES
#define MAX_TOPIC_LENGTH      30
#endif

#ifndef MQTT_BUF_SIZE
#define MQTT_BUF_SIZE 2048
#endif

#ifndef QUEUE_BUFFER_SIZE
#define QUEUE_BUFFER_SIZE 2048
#endif

#ifndef MAX_MQTT_CLIENTS
#define MAX_MQTT_CLIENTs 10
#endif

#ifndef MAX_SUBS_PER_REQ
#define MAX_SUBS_PER_REQ 16
#endif

#ifndef MQTT_KEEPALIVE
#define MQTT_KEEPALIVE 10 /*seconds*/
#endif

#ifndef MQTT_RECONNECT_TIMEOUT
#define MQTT_RECONNECT_TIMEOUT 5 /*seconds*/
#endif

#define PROTOCOL_NAMEv311 /*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/

#ifndef MQTT_ID
#define MQTT_ID "ESPBroker"
#endif

#ifdef DEBUG
#define MQTT_INFO    printf
#define MQTT_WARNING printf
#define MQTT_ERROR   printf
#else
#define MQTT_INFO    //
#define MQTT_WARNING //
#define MQTT_ERROR   //
#endif

