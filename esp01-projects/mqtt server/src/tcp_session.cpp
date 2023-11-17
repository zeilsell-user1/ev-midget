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

#include "tcp_session.h"
#include "tcp_server.h"

/*******************************************************************************
 * local null callback used to make sure any bugs are handled correctly. This
 * is outside of the class so that the callbacks can work across classes
 *******************************************************************************/

void nullCallback1(void *arg, void *obj)
{
    TCP_ERROR("callback called without initialisation");
}
void nullcallback2(void *arg, char *pdata, unsigned short len, void *obj)
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

TcpSession::TcpSession()
{
    this->sessionValid = false;
    this->sessionId = 0;
    this->sessionState = ESPCONN_NONE;
    this->sessionCbListener = nullptr;
    ip4_addr_set_zero(&(this->sessionConfig.remote_ip));
    this->sessionConfig.remote_port = 0;
    this->sessionConfig.sessionExpiryIntervalTimeout = 0;
    this->disconnectedCb = nullCallback1;
    this->incomingMessageCb = nullcallback2;
    this->messageSentCb = nullCallback1;
}

TcpSession::TcpSession(SessionState state,
                       ip_addr_t ipAddress,
                       unsigned short port,
                       espconn serverConn)
{
    this->sessionValid = true;
    this->sessionId = createUniqueIdentifier(ipAddress, port);
    ;
    this->sessionState = state;
    this->sessionCbListener = nullptr; // RJG THSI SIN'T SET!!!!
    ip4_addr_copy(this->sessionConfig.remote_ip, ipAddress);
    this->sessionConfig.remote_port = port;
    this->sessionConfig.sessionExpiryIntervalTimeout = 0;
    this->disconnectedCb = nullCallback1;
    this->incomingMessageCb = nullcallback2;
    this->messageSentCb = nullCallback1;

    espconn_regist_disconcb(&serverConn, localSessionDisconnectCb);
    espconn_regist_recvcb(&serverConn, localIncomingMessageCb);
    espconn_regist_sentcb(&serverConn, localMessageSentCb);
}

uintptr_t TcpSession::getSessionId()
{
    return this->sessionId;
}

bool TcpSession::isSessionValid()
{
    return this->sessionValid;
}

// Register the callback listener and the callbacls

void TcpSession::registerSessionCbListener(void *obj)
{
    this->sessionCbListener = obj;
}

void TcpSession::registerSessionDisconnectCb(void (*cb)(void *arg, void *))
{
    this->disconnectedCb = cb;
}

void TcpSession::registerIncomingMessageCb(void (*cb)(void *arg, char *pdata, unsigned short len, void *))
{
    this->incomingMessageCb = cb;
}

void TcpSession::registerMessageSentCb(void (*cb)(void *arg, void *))
{
    this->messageSentCb = cb;
}

// the methods that handle the callbacks.  

void TcpSession::sessionDisconnected(espconn *conn)
{
    this->disconnectedCb(conn, this->sessionCbListener);
}

void TcpSession::sessionIncomingMessage(espconn *conn, char *pdata, unsigned short length)
{
    this->incomingMessageCb(conn, pdata, length, this->sessionCbListener);
}

void TcpSession::sessionMessageSent(espconn *conn)
{
    this->messageSentCb(conn, this->sessionCbListener);
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