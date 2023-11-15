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

struct espconn serverConn;
esp_tcp tcpConfig;

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
 *******************************************************************************/

void espconnServerSessionConnectedCb(void *arg)
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.serverSessionConnected(arg);
}

void espconnClientSessionConnectedCb(void *arg)
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.clientSessionConnected(arg);
}

/*******************************************************************************
 * Class Implemenation - private
 *******************************************************************************/

TcpServer::TcpServer()
{
    this->status = NOT_STARTED;
    ip4_addr_set_zero(&this->ipAddress);
    this->port = 0;
    this->serverConnectedCb = nullCallback;
    this->clientConnectedCb = nullCallback;
    this->ownerObj = nullptr; // no owner object

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        this->tcpSessions[i] = std::make_shared<TcpSession>();
    }
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

bool TcpServer::startTcpServer(unsigned short port, void (*cb)(void *, std::shared_ptr<TcpSession>), void *obj)
{
    if (this->status == NOT_STARTED)
    {
        ip4_addr_set_zero(&this->ipAddress); // server mode has the IP address as zero

        this->status = SERVER;
        this->port = port;
        this->serverConnectedCb = (void (*)(void *, std::shared_ptr<TcpSession>))cb;
        this->ownerObj = obj; // this is the object that owns the callback

        // Set up server configuration
        serverConn.type = ESPCONN_TCP;
        serverConn.state = ESPCONN_NONE;
        serverConn.proto.tcp = &tcpConfig;
        serverConn.proto.tcp->local_port = port;

        espconn_regist_connectcb(&serverConn, espconnServerSessionConnectedCb);
        espconn_accept(&serverConn); // Enable server
        return true;
    }
    else
    {
        return false;
    }
}

bool TcpServer::startTcpClient(ip_addr_t ipAddress, unsigned short port, void (*cb)(void *, std::shared_ptr<TcpSession>), void *obj)
{
    if (this->status == NOT_STARTED)
    {
        this->status = CLIENT;
        this->ipAddress = ipAddress;
        this->port = port;
        this->serverConnectedCb = (void (*)(void *, std::shared_ptr<TcpSession>))cb;
        this->ownerObj = obj; // this is the object that owns the callback

        // Set the server's port
        serverConn.type = ESPCONN_TCP;
        serverConn.state = ESPCONN_NONE;
        serverConn.proto.tcp = &tcpConfig;
        serverConn.proto.tcp->remote_ip[0] = ip4_addr1(&ipAddress);
        serverConn.proto.tcp->remote_ip[1] = ip4_addr2(&ipAddress);
        serverConn.proto.tcp->remote_ip[2] = ip4_addr3(&ipAddress);
        serverConn.proto.tcp->remote_ip[3] = ip4_addr4(&ipAddress);
        serverConn.proto.tcp->remote_port = port;

        espconn_regist_connectcb(&serverConn, espconnClientSessionConnectedCb);
        espconn_connect(&serverConn);
        return true;
    }
    else
    {
        return false;
    }

    // TODO create the active session to IP and port
}

/*******************************************************************************
 * methods to handle the espconn events
 *******************************************************************************/

// Server connection callback. The ESPCONN has returned a session that is pointed
// to by the espconn struct in arg. The contents of this struct are transfered
// to a TCP session to abstract ESPCONN from the rest of the system. The TCP
// session is then passed to the owning object via the already-registered
// callback.

void TcpServer::serverSessionConnected(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;

    if ((conn->type == ESPCONN_TCP) && (conn->state == ESPCONN_CONNECT))
    {
        ip_addr_t ipAddress;
        IP4_ADDR(&ipAddress,
                 conn->proto.tcp->remote_ip[0],
                 conn->proto.tcp->remote_ip[1],
                 conn->proto.tcp->remote_ip[2],
                 conn->proto.tcp->remote_ip[3]);

        // IMPORTANT
        // the only way to distinguish between sessions with espconn library is
        // to use the address of conn. To avoid leaking the espconn structure
        // outside of the TCP Session and Server classes the address of conn is
        // converted to a uintptr_t and recorded as the session ID.

        uintptr_t sessionId = reinterpret_cast<uintptr_t>(conn);

        std::shared_ptr<TcpSession> tcpSession = std::make_shared<TcpSession>(sessionId,
                                                                              TcpSession::SERVER,
                                                                              TcpSession::ESPCONN_CONNECT,
                                                                              ipAddress,
                                                                              (unsigned short)conn->proto.tcp->remote_port,
                                                                              serverConn);

        // loop through the session list until an empty slot is found

        int i = 0;
        bool sessionValid = true;

        for (i = 0; (i < MAX_SESSIONS) && (sessionValid == true); i++)
        {
            sessionValid = this->tcpSessions[i]->isSessionValid();
        }

        if (i < MAX_SESSIONS)
        {
            // have space for a new session so store it callback the owner object
            this->tcpSessions[i] = tcpSession;
            this->serverConnectedCb(this->ownerObj, tcpSession);
        }
        else
        {
            // over the number of allowed sessions.
            // TODO Kill the ESPCONN session
        }
    }
}

// Client connection callback

void TcpServer::clientSessionConnected(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;

    if ((conn->type == ESPCONN_TCP) && (conn->state == ESPCONN_CONNECT))
    {
        ip_addr_t ipAddress;
        IP4_ADDR(&ipAddress,
                 conn->proto.tcp->remote_ip[0],
                 conn->proto.tcp->remote_ip[1],
                 conn->proto.tcp->remote_ip[2],
                 conn->proto.tcp->remote_ip[3]);

        // IMPORTANT
        // the only way to distinguish between sessions with espconn library is
        // to use the address of conn. To avoid leaking the espconn structure
        // outside of the TCP Session and Server classes the address of conn is
        // converted to a uintptr_t and recorded as the session ID.

        uintptr_t sessionId = reinterpret_cast<uintptr_t>(conn);

        std::shared_ptr<TcpSession> tcpSession = std::make_shared<TcpSession>(sessionId,
                                                                              TcpSession::SERVER,
                                                                              TcpSession::ESPCONN_CONNECT,
                                                                              ipAddress,
                                                                              (unsigned short)conn->proto.tcp->remote_port,
                                                                              serverConn);

        // loop through the session list until an empty slot is found

        int i = 0;
        bool sessionValid = true;

        for (i = 0; (i < MAX_SESSIONS) && (sessionValid == true); i++)
        {
            sessionValid = this->tcpSessions[i]->isSessionValid();
        }

        // A client may connect to multiple servers

        if (i < MAX_SESSIONS)
        {
            // have space for a new session so store it callback the owner object
            this->tcpSessions[i] = tcpSession;
            this->serverConnectedCb(this->ownerObj, tcpSession);
        }
        else
        {
            // over the number of allowed sessions.
            // TODO Kill the ESPCONN session
        }
    }
}

// espconn_regist_recvcb(&serverConn, serverRecvCb);
// espconn_regist_disconcb(&serverConn, serverDisconCb);