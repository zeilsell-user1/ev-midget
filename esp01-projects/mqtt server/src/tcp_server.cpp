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

#include "tcp_server.h"

/*******************************************************************************
 * serverConn is required to be valid for the entire life of the TCP server and
 * so is defined here as a global variable
 *******************************************************************************/

/*******************************************************************************
 * A dummy callback used in case the upper layer has not defined a real
 * callback. This makes sure any bugs are handled correctly. This callback
 * is outside of the class so that the callbacks can work across classes
 *******************************************************************************/

void nullCallback(void *obj, std::shared_ptr<TcpSession> tcpSession)
{
    TCP_ERROR("callback called without initialisation");
}

/*******************************************************************************
 * This callback is used by the ESPCONN software when a connect has been
 * received. It is outside the contect of the TcpServer and so uses the
 * getInstance to get the TcpServer object.
 *
 * Currenlty there is no difference bwetween a client session and an server
 * session so they both call the same handle for sessionConnected
 *******************************************************************************/

void espconnServerSessionConnectedCb(void *arg)
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.sessionConnected(arg);
}

void espconnClientSessionConnectedCb(void *arg)
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.sessionConnected(arg);
}

/*******************************************************************************
 * Class Implemenation - private
 *******************************************************************************/

TcpServer::TcpServer()
{
    ip4_addr_set_zero(&ipAddress_);
    port_ = 0;
    serverConnectedCb_ = nullCallback;
    clientConnectedCb_ = nullCallback;
    ownerObj_ = nullptr; // no owner object
}

TcpServer::~TcpServer()
{
}

/*******************************************************************************
 * Class Implemenation - public
 *******************************************************************************/

TcpServer &TcpServer::getInstance()
{
    static TcpServer instance; // This is thread-safe in C++11 and later
    return instance;
}

bool TcpServer::startTcpServer(unsigned short port, void (*cb)(void *, TcpSessionPtr), void *ownerObj)
{
    // TODO One server listening on this port 

    ip4_addr_set_zero(&ipAddress_); // server mode has the IP address as zero

    port_ = port;
    serverConnectedCb_ = (void (*)(void *, TcpSessionPtr))cb;
    ownerObj_ = ownerObj; // this is the object that owns the callback

    // Set up server configuration
    serverConn_.type = ESPCONN_TCP;
    serverConn_.state = ESPCONN_NONE;
    serverConn_.proto.tcp = &tcpConfig_;
    serverConn_.proto.tcp->local_port = port;

    espconn_regist_connectcb(&serverConn_, espconnServerSessionConnectedCb);
    // TODO espconn_regist_reconcb(&serverConn, espconnClientSessionReconnectedCb);

    signed char rc = espconn_accept(&serverConn_); // Enable server

    switch (rc)
    {
    case 0:
        MQTT_INFO("espconn_connect returned success");
        return true;
    case ESPCONN_MEM:
        MQTT_WARNING("espconn_connect returned ESPCONN_MEM");
        return false;
    case ESPCONN_ISCONN:
        MQTT_WARNING("espconn_connect returned MQTT_WARNING");
        return false;
    case ESPCONN_ARG:
        MQTT_WARNING("espconn_connect returned MQTT_WARNING");
        return false;
    default:
        MQTT_WARNING("espconn_connect returned %d", rc);
        return false;
    }
}

bool TcpServer::startTcpClient(ip_addr_t ipAddress, unsigned short port, void (*cb)(void *, TcpSessionPtr), void *ownerObj)
{
    
    // TODO One client connecting on this port and IP address combination
    
    ipAddress_ = ipAddress;
    port_ = port;
    serverConnectedCb_ = (void (*)(void *, std::shared_ptr<TcpSession>))cb;
    ownerObj_ = ownerObj; // this is the object that owns the callback

    // Set the server's port
    serverConn_.type = ESPCONN_TCP;
    serverConn_.state = ESPCONN_NONE;
    serverConn_.proto.tcp = &tcpConfig_;
    serverConn_.proto.tcp->remote_ip[0] = ip4_addr1(&ipAddress);
    serverConn_.proto.tcp->remote_ip[1] = ip4_addr2(&ipAddress);
    serverConn_.proto.tcp->remote_ip[2] = ip4_addr3(&ipAddress);
    serverConn_.proto.tcp->remote_ip[3] = ip4_addr4(&ipAddress);
    serverConn_.proto.tcp->remote_port = port;

    espconn_regist_connectcb(&serverConn_, espconnClientSessionConnectedCb);
    // TODO espconn_regist_reconcb(&serverConn, espconnClientSessionReconnectedCb);

    signed char rc = espconn_connect(&serverConn_);

    switch (rc)
    {
    case 0:
        MQTT_INFO("espconn_connect returned success");
        return true;
    case ESPCONN_RTE:
        MQTT_WARNING("espconn_connect returned ESPCONN_RTE");
        return false;
    case ESPCONN_MEM:
        MQTT_WARNING("espconn_connect returned ESPCONN_MEM");
        return false;
    case ESPCONN_ISCONN:
        MQTT_WARNING("espconn_connect returned MQTT_WARNING");
        return false;
    case ESPCONN_ARG:
        MQTT_WARNING("espconn_connect returned MQTT_WARNING");
        return false;
    default:
        MQTT_WARNING("espconn_connect returned %d", rc);
        return false;
    }
}

/*******************************************************************************
 * methods to handle the espconn events
 *******************************************************************************/

// Server connection callback. The ESPCONN has returned a session that is pointed
// to by the espconn struct in arg. The contents of this struct are transfered
// to a TCP session to abstract ESPCONN from the rest of the system. The TCP
// session is then passed to the owning object via the already-registered
// callback.

void TcpServer::sessionConnected(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;

    if ((conn->type == ESPCONN_TCP) && (conn->state == ESPCONN_CONNECT))
    {
        TcpSessionPtr tcpSession = std::make_shared<TcpSession>(TcpSession::ESPCONN_CONNECT,
                                                                TcpSession::convertIpAddress(conn->proto.tcp->remote_ip),
                                                                (unsigned short)conn->proto.tcp->remote_port,
                                                                conn);

        TcpSession::SessionId sessionId = tcpSession->getSessionId();

        if (!this->addSession(sessionId, tcpSession))
        {
            espconn_abort(conn); // couldn't add session so abort the TCP session
        }
    }
}

bool TcpServer::addSession(TcpSession::SessionId sessionId, const TcpSessionPtr &session)
{
    // Check if the identifier is already in use
    if (tcpSessions_.find(sessionId) == tcpSessions_.end())
    {
        if (tcpSessions_.size() < MAX_TCP_SESSIONS)
        {
            tcpSessions_[sessionId] = session;
            MQTT_INFO("added session");
            return true;
        }
        else
        {
            MQTT_INFO("too many sessions");
            return false;
        }
    }

    MQTT_INFO("session already in the array");
    return true;
}

void TcpServer::removeSession(TcpSession::SessionId sessionId)
{
    auto it = tcpSessions_.find(sessionId);
    if (it != tcpSessions_.end())
    {
        tcpSessions_.erase(it);
        MQTT_INFO("deleted session");
    }
    else
    {
        MQTT_INFO("session not found in the map");
    }
}

TcpServer::TcpSessionPtr TcpServer::getSession(TcpSession::SessionId sessionId)
{
    auto it = tcpSessions_.find(sessionId);
    if (it != tcpSessions_.end())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

// espconn_regist_recvcb(&serverConn, serverRecvCb);
// espconn_regist_disconcb(&serverConn, serverDisconCb);