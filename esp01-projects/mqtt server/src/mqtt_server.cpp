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

#include "mqtt_server.h"

/*
 ******************************************************************************
 * Callback functions to receive data and events from the TCP server. These
 * are outside the class so that the TCP server remains indifferent to the
 * details of the client calling it.
 ******************************************************************************
 */

void tcpSessionConnectCb(void *obj, std::shared_ptr<TcpSession> tcpSession)
{
    MqttServer *mqttServer = static_cast<MqttServer *>(obj);
    mqttServer->handleTcpSessionConnect(tcpSession);
}

/*
 * ****************************************************************************
 * Start of the public classes
 * ****************************************************************************
 */

MqttServer::MqttServer()
{
    ip4_addr_set_any(&this->ipAddress);
    this->port = 0;

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        this->sessionMapping[i].mappingValid = false;
        this->sessionMapping[i].mqttSession = nullptr;
        this->sessionMapping[i].tcpSession = nullptr;
    }
}

MqttServer::MqttServer(ip_addr_t ipAddress, unsigned short port) // client
{
    this->ipAddress = ipAddress;
    this->port = port;

    TcpServer& tcpServer = TcpServer::getInstance();
    tcpServer.startTcpClient(this->ipAddress, this->port, tcpSessionConnectCb, (void *)this);
    
    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        this->sessionMapping[i].mappingValid = false;
        this->sessionMapping[i].mqttSession = nullptr;
        this->sessionMapping[i].tcpSession = nullptr;
    }
}

MqttServer::MqttServer(unsigned short port) // server
{
    ip4_addr_set_any(&this->ipAddress);
    this->port = port;
    
    TcpServer& tcpServer = TcpServer::getInstance();
    tcpServer.startTcpServer(this->port, tcpSessionConnectCb, (void *)this);
    
    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        this->sessionMapping[i].mappingValid = false;
        this->sessionMapping[i].mqttSession = nullptr;
        this->sessionMapping[i].tcpSession = nullptr;
    }
}

void MqttServer::handleTcpSessionConnect(std::shared_ptr<TcpSession> tcpSession)
{
    int i = 0;

    for (i = 0; i < MAX_SESSIONS; i++)
    {
        if (this->sessionMapping[i].mappingValid == false)
        {
            break;
        }
    }

    if (i < MAX_SESSIONS)
    {
        this->sessionMapping[i].tcpSession = tcpSession;
        this->sessionMapping[i].mqttSession = std::make_unique<MqttSession>(tcpSession);
        this->sessionMapping[i].mappingValid = true; 
    }
}
