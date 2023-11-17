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

// MQTT Broker Configuraion

#ifndef MQTT_BROKER_PORT
#define MQTT_BROKER_PORT 1883
#endif

#ifndef MAX_MQTT_SESSIONS
#define MAX_MQTT_SESSIONS 10
#endif

#ifndef MAX_SUBSCRIPTIONS
#define MAX_SUBSCRIPTIONS 30
#endif

#ifndef MAX_RETAINED_TOPICS
#define MAX_RETAINED_TOPICS 30
#endif

#ifndef MAX_TOPIC_LIST_ENTRIES
#define MAX_TOPIC_LIST_ENTRIES 30
#endif

#ifndef MAX_TOPIC_LENGTH // YES
#define MAX_TOPIC_LENGTH 50
#endif

#ifndef MAX_TOPICS_IN_SUBSCRIBE
#define MAX_TOPICS_IN_SUBSCRIBE 5
#endif

#ifndef MAX_MSG_LENGTH // YES
#define MAX_MSG_LENGTH 200
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

#define MQTT_DEBUG 1
#ifdef MQTT_DEBUG
#define MQTT_INFO(format, ...)     \
    printf(format, ##__VA_ARGS__); \
    printf("\n")
#define MQTT_WARNING(format, ...)  \
    printf(format, ##__VA_ARGS__); \
    printf("\n")
#define MQTT_ERROR(format, ...)    \
    printf(format, ##__VA_ARGS__); \
    printf("\n")
#else
#define MQTT_INFO(format, ...)
#define MQTT_WARNING(format, ...)
#define MQTT_ERROR(format, ...)
#endif

// TCP Server Configuraiton

#ifndef MAX_TCP_SESSIONS
#define MAX_TCP_SESSIONS 10
#endif

#define TCP_DEBUG 1
#ifdef TCP_DEBUG
#define TCP_INFO(format, ...)      \
    printf(format, ##__VA_ARGS__); \
    printf("\n")
#define TCP_WARNING(format, ...)   \
    printf(format, ##__VA_ARGS__); \
    printf("\n")
#define TCP_ERROR(format, ...)     \
    printf(format, ##__VA_ARGS__); \
    printf("\n")
#else
#define TCP_INFO(format, ...)
#define TCP_WARNING(format, ...)
#define TCP_ERROR(format, ...)
#endif
