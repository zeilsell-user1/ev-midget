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

#include <memory>     // For std::shared_ptr, std::unique_ptr
#include <stdexcept>  // For std::runtime_error

#ifdef ESP8266
#include <lwip/ip.h>
#include "espconn.h"
#else
#include "../test/ip_addr.h"
#include "../test/ip4_addr.h"
#include "../test/ip.h"
#include "../test/espconn.h"
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

class TcpSErver;

class TcpSession  : public std::enable_shared_from_this<TcpSession>
{
public:
    using TcpSessionPtr = std::shared_ptr<TcpSession>;
    struct SessionConfig
    {
        ip_addr_t remote_ip;
        unsigned short remote_port;
        unsigned long sessionExpiryIntervalTimeout;
    };

    using SessionId = std::size_t;

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
    
    enum sendResult
    {
        SEND_OK,
        RETRY,
        FAILED_ABORTED
    };

    bool isSessionValid();
    SessionId getSessionId();
    void disconnectSession();
    sendResult sendMessage(unsigned char *pData, unsigned short len);

    // these methods are used to register callbacks for TCP session events. There
    // may be multiple sessions, adn each session may be 'owned' by a different instance 
    // of whatever is calling this session. Therefore the void *obj registered as the
    // sessionCbListener is passed as a paremeter int he callback. 
    //
    // The callbacks do not know anything about the underlying espconnlibrary, 
    // the session provide abstraction of the firmware

    void registerSessionCbListener(void *obj, TcpSessionPtr session);
    void registerSessionDisconnectCb(void (*cb)(void *obj, TcpSessionPtr session));
    void registerIncomingMessageCb(void (*cb)(void *obj, char *pData, unsigned short len, TcpSessionPtr session));
    void registerMessageSentCb(void (*cb)(void *obj, TcpSessionPtr session));
    
    // these are the callbacks that the session registeres with the espconn firmware.

    void sessionDisconnected(espconn *);
    void sessionIncomingMessage(espconn *, char *, unsigned short);
    void sessionMessageSent(espconn *);

    // static utility methods 

    static ip_addr_t convertIpAddress(unsigned char *);
    static SessionId createUniqueIdentifier(const ip_addr_t& ipAddress, int port);

    // A drawback of using the RAII (Resource Acquisition Is Initialization) principle is that
    // shared_ptr and unique_ptr both need to have access to the constructir and destructor for
    // the class. Unfortunately this means they need to be public, which sucks. Please DO NOT
    // call TcpSession directly, a session is created and managed by TcpServer

    TcpSession(ip_addr_t ipAddress, unsigned short port, espconn *serverConn); 
    ~TcpSession();

private:
    TcpSession() = default;
    void (*disconnectedCb)(void *obj, TcpSessionPtr session);
    void (*incomingMessageCb)(void *obj, char *pdata, unsigned short len, TcpSessionPtr session);
    void (*messageSentCb)(void *obj, TcpSessionPtr session);

private:
    bool sessionValid_;
    SessionId sessionId_;
    SessionState sessionState_;
    void * sessionCbListener_;
    SessionConfig sessionConfig_;
    espconn serverConn_;
};

#endif // TCP_SESSION_H