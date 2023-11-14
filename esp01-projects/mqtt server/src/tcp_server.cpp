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
 * local null callback used to make sure any bugs are handled correctly. THis
 * is outside of the class so that the callbacks can work across classes
 *******************************************************************************/

void nullCallback1(void *obj, TcpSession *tcpSession)
{
    TCP_ERROR("callback called without initialisation");
}

/*******************************************************************************
 * This callback is used by the ESPCONN software when a connect has been
 * recevied. It is outside the contect of the TcpServer ans so used the
 * getInstance to get the TcpServer object.
 *******************************************************************************/

void localServerConnectedCb(void *arg)
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.serverConnectCallback(arg);
}

void localClientConnectedCb(void *arg)
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.clientConnectCallback(arg);
}

/*******************************************************************************
 * Class Implemenation - private
 *******************************************************************************/

TcpServer::TcpServer()
{
    this->status = NOT_STARTED;
    ip4_addr_set_any(&this->ipAddress);
    this->port = 0;
    this->connectedCb = nullCallback1;
    this->obj = nullptr;
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

bool TcpServer::startTcpServer(unsigned short port, void (*cb)(void *, TcpSession *), void *obj)
{
    if (this->status == NOT_STARTED)
    {
        this->status = SERVER;
        ip4_addr_set_zero(&this->ipAddress); // server mode has the IP address as zero
        this->port = port;
        this->serverConnectCallback = (void (*)(void *, TcpSession *tcpSession))cb;
        this->obj = obj; // this is the object that owns the callback

        // Set up server configuration
        this->serverConn.type = ESPCONN_TCP;
        this->serverConn.state = ESPCONN_NONE;
        this->serverConn.proto.tcp = &(this->tcpConfig);
        this->serverConn.proto.tcp->local_port = port;

        // Register callback functions
        espconn_regist_connectcb(&serverConn, localServerConnectedCb);
        // espconn_regist_recvcb(&serverConn, serverRecvCb);
        // espconn_regist_disconcb(&serverConn, serverDisconCb);

        espconn_accept(&serverConn); // Enable server
        return true;
    }
    else
    {
        return false;
    }
}

bool TcpServer::startTcpClient(ip_addr_t ipAddress, unsigned short port, void (*cb)(void *, TcpSession *), void *obj)
{
    if (this->status == NOT_STARTED)
    {
        this->status = CLIENT;
        this->ipAddress = ipAddress;
        this->port = port;
        this->connectedCb = (void (*)(void *, TcpSession *tcpSession))cb;
        this->obj = obj; // this is the object that owns the callback
        ipaddr_aton(serverIp, &(client->remote_ip));

        // Set the server's port
        client->remote_port = serverPort;

        // Perform the connection
        espconn_connect(client);
        espconn_regist_connectcb(&serverConn, localClientConnectedCb);
        return true;
    }
    else
    {
        return false;
    }

    // TODO create the active session to IP and port
}

/*******************************************************************************
 * callbacks to handle the espconn events
 *******************************************************************************/

// Server connection callback

void TcpServer::serverConnectCallback(void *arg)
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

        TcpSession tcpSerssion = tcpSession(TcpSession::SERVER,
                                            TcpSession::ESPCONN_CONNECT,
                                            ipAddress,
                                            (unsigned int)conn->proto.tcp->remote_port);

        TcpSession tcpSession = TcpSession(tcpSession);
        TcpServer &tcpServer = TcpServer::getInstance();
        tcpServer.connectedServerCb(tcpServer.obj, &tcpSession);
    }
}

// Client connection callback

void TcpServer::clientConnectCallback(void *arg)
{
        ip_addr_t ipAddress;
        IP4_ADDR(&ipAddress,
                 conn->proto.tcp->remote_ip[0],
                 conn->proto.tcp->remote_ip[1],
                 conn->proto.tcp->remote_ip[2],
                 conn->proto.tcp->remote_ip[3]);

        TcpSession tcpSerssion = tcpSession(TcpSession::CLIENT,
                                            conn->state,
                                            TcpSession::ESPCONN_CONNECT,
                                            ipAddress,
                                            (unsigned int)conn->proto.tcp->remote_port);

        TcpSession tcpSession = TcpSession(tcpSession);
        TcpServer &tcpServer = TcpServer::getInstance();
        tcpServer.connectedClientCb(tcpServer.obj, &tcpSession);
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
