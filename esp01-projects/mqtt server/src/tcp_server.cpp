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
 * local null callback used to make sure any bugs are handled correctly. This
 * is outside of the class so that the callbacks can work across classes
 *******************************************************************************/

void nullCallback(void *obj, long tcpSessionId)
{
    TCP_ERROR("callback called without initialisation");
}

/*******************************************************************************
 * This callback is used by the ESPCONN software when a connect has been
 * received. It is outside the contect of the TcpServer and so uses the
 * getInstance to get the TcpServer object.
 *******************************************************************************/

void localServerSessionConnectedCb(void *arg)
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.serverSessionConnected(arg);
}

void localClientSessionConnectedCb(void *arg)
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
    ip4_addr_set_any(&this->ipAddress);
    this->port = 0;
    this->serverConnectedCb = nullCallback;
    this->clientConnectedCb = nullCallback;
    this->obj = nullptr;

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        this->tcpSessions[i] = &TcpSession(); // initialise to invalid
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

bool TcpServer::startTcpServer(unsigned short port, void (*cb)(void *, long), void *obj)
{
    if (this->status == NOT_STARTED)
    {
        ip4_addr_set_zero(&this->ipAddress); // server mode has the IP address as zero

        this->status = SERVER;
        this->port = port;
        this->serverConnectedCb = (void (*)(void *, long tcpSessionId))cb;
        this->obj = obj; // this is the object that owns the callback

        // Set up server configuration
        serverConn.type = ESPCONN_TCP;
        serverConn.state = ESPCONN_NONE;
        serverConn.proto.tcp = &tcpConfig;
        serverConn.proto.tcp->local_port = port;

        espconn_regist_connectcb(&serverConn, localServerSessionConnectedCb);
        espconn_accept(&serverConn); // Enable server
        return true;
    }
    else
    {
        return false;
    }
}

bool TcpServer::startTcpClient(ip_addr_t ipAddress, unsigned short port, void (*cb)(void *, long), void *obj)
{
    if (this->status == NOT_STARTED)
    {
        this->status = CLIENT;
        this->ipAddress = ipAddress;
        this->port = port;
        this->clientConnectedCb = (void (*)(void *, long tcpSessionId))cb;
        this->obj = obj; // this is the object that owns the callback

        // Set the server's port
        serverConn.type = ESPCONN_TCP;
        serverConn.state = ESPCONN_NONE;
        serverConn.proto.tcp = &tcpConfig;
        serverConn.proto.tcp->remote_ip[0] = ip4_addr1(&ipAddress);
        serverConn.proto.tcp->remote_ip[1] = ip4_addr2(&ipAddress);
        serverConn.proto.tcp->remote_ip[2] = ip4_addr3(&ipAddress);
        serverConn.proto.tcp->remote_ip[3] = ip4_addr4(&ipAddress);
        serverConn.proto.tcp->remote_port = port;

        espconn_regist_connectcb(&serverConn, localClientSessionConnectedCb);
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

// Server connection callback

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
        // converted to a long and recorded as the session ID.

        long sessionId = (long)conn;

        TcpSession tcpSession = TcpSession(sessionId,
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
            this->tcpSessions[i] = &tcpSession;
        }

        TcpServer &tcpServer = TcpServer::getInstance();
        tcpServer.serverConnectedCb(tcpServer.obj, sessionId);
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
        // converted to a long and recorded as the session ID.

        long sessionId = (long)conn;

        TcpSession tcpSession = TcpSession(sessionId,
                                            TcpSession::CLIENT,
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
            this->tcpSessions[i] = &tcpSession;
        }

        TcpServer &tcpServer = TcpServer::getInstance();
        tcpServer.clientConnectedCb(tcpServer.obj, sessionId);
    }
}

// TODO the active or passive session will call the connect callback using the
// this->connectedCb(this->obj, tcp-session);

// #include "ets_sys.h"
// #include "osapi.h"
// #include "user_interface.h"
// #include "espconn.h"

// // Callback function when the server receives data from a client
// void serverRecvCb(void *arg, char *data, unsigned short len) {
//     struct espconn *conn = (struct espconn *)arg;
//     os_printf("Received data from client: %s\n", data);

//     // You can add your processing logic here

//     // Send a response back to the client
//     espconn_sent(conn, "Hello, client!", 14);
// }

// // Callback function when a client disconnects from the server
// void serverDisconCb(void *arg) {
//     os_printf("Client disconnected\n");
// }

// espconn_regist_recvcb(&serverConn, serverRecvCb);
// espconn_regist_disconcb(&serverConn, serverDisconCb);