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

#include <string.h>
#include "defaults.h"
#include "tcp_session.h"

void nullCallback1(void *arg)
{
    TCP_ERROR("callback called without initialisation");
}
void nullcallback2(void *arg, char *pdata, unsigned short len)
{
    TCP_ERROR("callback called without initialisation");
}

TcpSession::TcpSession()
{
    this->sessionValid = false;
    //struct espconn *pesp_conn;
    memset(this->IPAddress, '\0', 4);
    this->sessionExpiryIntervalTimeout = 0;
    this->connectedCb = nullCallback1;
    this->disconnectedCb = nullCallback1;
    this->incomingMessageCb = nullcallback2;
    this->messageSentCb = nullCallback1;
    this->messageAcknowledgedCb = nullCallback1;
}

bool TcpSession::registerSessionConnect_cb(void *cb, void *obj)
{
    this->connectedCb = (void (*)(void *arg))cb;
}

bool TcpSession::registerSessionDisconnect_cb(void *cb, void *obj)
{
    this->disconnectedCb = (void (*)(void *arg))cb;
}

bool TcpSession::registerIncomingMessage_cb(void *cb, void *obj)
{
    this->incomingMessageCb = (void (*)(void *arg, char *pdata, unsigned short len))cb;
}

bool TcpSession::registerMessageSent_cb(void *cb, void *obj)
{
    this->messageSentCb = (void (*)(void *arg))cb;
}

bool TcpSession::regsiterMessageAcknowledged_cb(void *cb, void *obj)
{
    this->messageAcknowledgedCb = (void (*)(void *arg))cb;
}

bool TcpSession::startTcpServer(unsigned short port)
{

}

bool TcpSession::startTcpClient(unsigned char* ipAddress, unsigned short port)
{

}

/*************************************************************************/
/*
 * Private Functions
 */
