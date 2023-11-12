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
#else
#include "../test/test_mqtt_server/ip.h"
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
//#include "espconn.h"

class TcpSession
{
    private:
        bool sessionValid;
        //struct espconn *pesp_conn;
        ip_addr_t ipAddress;
        unsigned long sessionExpiryIntervalTimeout;
        
        void (*connectedCb)(void *arg);
        void (*disconnectedCb)(void *arg);
        void (*incomingMessageCb)(void *arg, char *pdata, unsigned short len);
        void (*messageSentCb)(void *arg);
        void (*messageAcknowledgedCb)(void *arg);

    public:
        TcpSession();
        TcpSession(ip_addr_t ipAddress, unsigned short port); // client
        TcpSession(unsigned short port); // server

        bool registerSessionConnect_cb(void *cb, void *obj);
        bool registerSessionDisconnect_cb(void *cb, void *obj);
        bool registerIncomingMessage_cb(void *cb, void *obj);
        bool registerMessageSent_cb(void *cb, void *obj);
        bool regsiterMessageAcknowledged_cb(void *cb, void *obj);

        bool startTcpServer(unsigned short port);
        bool startTcpClient(ip_addr_t ipAddress, unsigned short port);
};

#endif //TCP_SESSION_H