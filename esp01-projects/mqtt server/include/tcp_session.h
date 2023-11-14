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

#ifndef TCP_SESSION_H
#define TCP_SESSION_H

#ifdef ESP8266
#include <lwip/ip.h>
#include "espconn.h"
#else
#include "../test/test_mqtt_server/ip.h"
#include "../test/test_mqtt_server/espconn.h"
#endif
#include "defaults.h"

// In the context of TCP (Transmission Control Protocol), a "TCP session"
// usually encompasses the entire lifespan of a TCP connection, from its
// establishment to its termination. This includes the three-way handshake
// for connection setup, data transmission, and the four-way handshake for
// connection teardown.
//
// So, when you say "TCP session," it generally implies the complete life
// cycle of a TCP connection, which includes the setup, data exchange, and
// teardown phases.
// #include "espconn.h"

class TcpSession
{
public:
    // Define a structure to hold client-specific information
    struct ClientConfig
    {
        ip_addr_t remote_ip;
        unsigned short remote_port;
        int clientData; // Example client-specific data
    };

    struct ServerConfig
    {
        ip_addr_t remote_ip;
        unsigned short remote_port;
        int clientData; // Example client-specific data
    };

    enum SessionType
    {
        NOT_STARTED,
        CLIENT,
        SERVER
    };

    enum SessionState
    {
        ESPCONN_NONE,
        ESPCONN_WAIT,
        ESPCONN_LISTEN,
        ESPCONN_CONNECT,
        ESPCONN_WRITE,
        ESPCONN_READ,
        ESPCONN_CLOSE
    };

    TcpSession();
    TcpSession(long sessionId,
               enum SessionType type,
               enum SessionState state,
               ip_addr_t ipAddress, 
               unsigned short port,
               espconn serverConn); 

    bool isSessionValid();
    long getSessionId();

    bool registerSessionDisconnectCb(void (*cb)(void *, void *), void *obj);
    bool registerIncomingMessageCb(void (*cb)(void *, char *, unsigned short, void *), void *obj);
    bool registerMessageSentCb(void (*cb)(void *, void *), void *obj);
    
private:
    bool sessionValid;
    long sessionId;
    enum SessionState state;
    enum SessionType type;
    ip_addr_t ipAddress;
    unsigned long sessionExpiryIntervalTimeout;

    void (*disconnectedCb)(void *obj, void *arg);
    void (*incomingMessageCb)(void *obj, void *arg, char *pdata, unsigned short len);
    void (*messageSentCb)(void *obj, void *arg);

};

#endif // TCP_SESSION_H