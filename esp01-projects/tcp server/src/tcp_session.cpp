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

#include <string>
#include "tcp_session.h"
#include "tcp_server.h"

/*******************************************************************************
 * local null callback used to make sure any bugs are handled correctly. This
 * is outside of the class so that the callbacks can work across classes
 *******************************************************************************/

void nullCallback1(void *obj, TcpSession::TcpSessionPtr session)
{
    TCP_ERROR("callback called without initialisation");
}
void nullcallback2(void *obj, char *pdata, unsigned short len, TcpSession::TcpSessionPtr session)
{
    TCP_ERROR("callback called without initialisation");
}

/*
 ******************************************************************************
 * Callback functions to receive data and events from ESPCONN library. These
 * are outside the class so that the ESPCONN library remains indifferent to
 * the details of the client calling it.
 ******************************************************************************
 */

void localSessionDisconnectCb(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;
    const ip_addr_t ipAddress = TcpSession::convertIpAddress(conn->proto.tcp->remote_ip);
    TcpSession::SessionId sessionId = TcpSession::createUniqueIdentifier(ipAddress, conn->proto.tcp->remote_port);

    TcpServer &tcpServer = TcpServer::getInstance();
    std::shared_ptr<TcpSession> tcpSession = tcpServer.getSession(sessionId);
    tcpSession->sessionDisconnected(conn);
}

void localIncomingMessageCb(void *arg, char *pdata, unsigned short len)
{
    struct espconn *conn = (struct espconn *)arg;
    const ip_addr_t ipAddress = TcpSession::convertIpAddress(conn->proto.tcp->remote_ip);
    TcpSession::SessionId sessionId = TcpSession::createUniqueIdentifier(ipAddress, conn->proto.tcp->remote_port);

    TcpServer &tcpServer = TcpServer::getInstance();
    std::shared_ptr<TcpSession> tcpSession = tcpServer.getSession(sessionId);
    tcpSession->sessionIncomingMessage(conn, pdata, len);
}

void localMessageSentCb(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;
    const ip_addr_t ipAddress = TcpSession::convertIpAddress(conn->proto.tcp->remote_ip);
    TcpSession::SessionId sessionId = TcpSession::createUniqueIdentifier(ipAddress, conn->proto.tcp->remote_port);

    TcpServer &tcpServer = TcpServer::getInstance();
    std::shared_ptr<TcpSession> tcpSession = tcpServer.getSession(sessionId);
    tcpSession->sessionMessageSent(conn);
}

/*******************************************************************************
 * Class Implemenation - public
 *******************************************************************************/

TcpSession::TcpSession(ip_addr_t ipAddress, unsigned short port, espconn *conn)
{
    sessionValid_ = true;
    sessionId_ = createUniqueIdentifier(ipAddress, port);
    sessionState_ = ESPCONN_CONNECT,
    sessionCbListener_ = nullptr;
    ip4_addr_copy(sessionConfig_.remote_ip, ipAddress);
    sessionConfig_.remote_port = port;
    sessionConfig_.sessionExpiryIntervalTimeout = 0;
    memcpy(&(serverConn_), conn, sizeof(espconn));
    this->disconnectedCb = nullCallback1;
    this->incomingMessageCb = nullcallback2;
    this->messageSentCb = nullCallback1;

    if (espconn_regist_disconcb(&serverConn_, localSessionDisconnectCb) == 0)
    {
        if (espconn_regist_recvcb(&serverConn_, localIncomingMessageCb) == 0)
        {
            if (espconn_regist_sentcb(&serverConn_, localMessageSentCb) == 0)
            {
                TCP_INFO("Session registered all callbacks");
                return;
            }
            else
            {
                TCP_WARNING("failed to register sent callback");
            }
        }
        else
        {
            TCP_WARNING("failed to register incoming message callback");
        }
    }
    else
    {
        TCP_WARNING("failed to register disconnect callback");
    }

    // as one of the registrations didn't work then abort the connection

    espconn_abort(conn); // couldn't add session so abort the TCP session
    sessionValid_ = false;
    throw std::runtime_error("Failed to initialize TcpSession");
    return;
}

TcpSession::~TcpSession()
{
}

uintptr_t TcpSession::getSessionId()
{
    return sessionId_;
}

bool TcpSession::isSessionValid()
{
    return sessionValid_;
}

// Register the callback listener and the callbacls

void TcpSession::registerSessionCbListener(void *obj, TcpSessionPtr session)
{
    sessionCbListener_ = obj;
}

void TcpSession::registerSessionDisconnectCb(void (*cb)(void *, TcpSessionPtr session))
{
    this->disconnectedCb = cb;
}

void TcpSession::registerIncomingMessageCb(void (*cb)(void *, char *pdata, unsigned short len, TcpSessionPtr session))
{
    this->incomingMessageCb = cb;
}

void TcpSession::registerMessageSentCb(void (*cb)(void *, TcpSessionPtr session))
{
    this->messageSentCb = cb;
}

// the methods that handle the callbacks.

void TcpSession::sessionDisconnected(espconn *conn)
{
    this->disconnectedCb(sessionCbListener_, (TcpSessionPtr)this);
}

void TcpSession::sessionIncomingMessage(espconn *conn, char *pdata, unsigned short length)
{
    this->incomingMessageCb(sessionCbListener_, pdata, length, (TcpSessionPtr)this);
}

void TcpSession::sessionMessageSent(espconn *conn)
{
    this->messageSentCb(sessionCbListener_, (TcpSessionPtr)this);
}

// stiatic utility methods

ip_addr_t TcpSession::convertIpAddress(unsigned char *ipAddr)
{
    ip_addr_t ipAddress;
    IP4_ADDR(&ipAddress, ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    return ipAddress;
}

/*******************************************************************************
 * Class Implemenation - private
 *******************************************************************************/

TcpSession::SessionId TcpSession::createUniqueIdentifier(const ip_addr_t &ipAddress, int port)
{
    // Convert IP address to string
    std::string ipString = std::to_string(ipAddress.addr);

    // Concatenate IP address and port as a string
    std::string combinedString = std::string(ipString) + ":" + std::to_string(port);

    // Hash the combined string to produce a unique identifier
    std::hash<std::string> hasher;
    return hasher(combinedString);
}