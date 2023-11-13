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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <iostream>
#include <mutex>

#ifdef ESP8266
#include <lwip/ip.h>
#include "espconn.h"
#else
#include "../test/test_mqtt_server/ip.h"
#include "../test/test_mqtt_server/espconn.h"
#endif

#include "defaults.h"
#include "tcp_session.h"

// A TCP (Transmission Control Protocol) server is a network service that
// operates on the transport layer of the Internet Protocol (IP) suite.
// Its primary function is to listen for incoming TCP connections from
// multiple clients and respond to their requests. Additionally, a TCP
// server may initiate outgoing connections to other servers, allowing it
// to act not only as a responder to client requests but also as an initiator
// of communication.
//
// The TCP protocol is implemented in the ESP8266 (ESP01) device. This
// class represents the server within the MQTT server application.

class TcpServer
{
private:
    enum ServerType {
        NOT_STARTED,
        CLIENT,
        SERVER
    };

    ServerType status;
    ip_addr_t ipAddress;
    unsigned short port;
    struct espconn serverConn;
    esp_tcp tcpConfig;
    void *obj; // the callback object

    TcpServer();
    ~TcpServer();
    TcpServer(const TcpServer &) = delete;
    TcpServer &operator=(const TcpServer &) = delete;
    void (*connectedCb)(void *obj, TcpSession *session);

public:
    static TcpServer &getInstance();
    bool startTcpServer(unsigned short port, void (*cb)(void *, TcpSession *), void *obj);
    bool startTcpClient(ip_addr_t ipAddress, unsigned short port, void (*cb)(void *, TcpSession *), void *obj);

    void serverConnectedCb(void *arg);
};

#endif // TCP_SERVER_H