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

/*******************************************************************************
 * local null callback used to make sure any bugs are handled correctly. This
 * is outside of the class so that the callbacks can work across classes
 *******************************************************************************/

void nullCallback1(void *obj, void *arg)
{
    TCP_ERROR("callback called without initialisation");
}
void nullcallback2(void *obj, void *arg, char *pdata, unsigned short len)
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

}

void localIncomingMessageCb(void *arg, char *pdata, unsigned short len)
{
}

void localMessageSentCb(void *arg)
{
}

/*******************************************************************************
 * Class Implemenation - public
 *******************************************************************************/

TcpSession::TcpSession()
{
    this->sessionValid = false;
    ip4_addr_set_any(&this->ipAddress);
    this->sessionExpiryIntervalTimeout = 0;
    this->disconnectedCb = nullCallback1;
    this->incomingMessageCb = nullcallback2;
    this->messageSentCb = nullCallback1;
}

TcpSession::TcpSession(long sessionId,
                       enum SessionType type,
                       enum SessionState state,
                       ip_addr_t ipAddress, 
                       unsigned short port, 
                       espconn serverConn)
{
    this->sessionId = sessionId;
    this->sessionValid = true;
    this->state = state;
    this->type = type;
    ip4_addr_copy(this->ipAddress, ipAddress);
    this->sessionExpiryIntervalTimeout = 0;
    this->disconnectedCb = nullCallback1;
    this->incomingMessageCb = nullcallback2;
    this->messageSentCb = nullCallback1;

    espconn_regist_disconcb(&serverConn, localSessionDisconnectCb);
    espconn_regist_recvcb(&serverConn, localIncomingMessageCb);
    espconn_regist_sentcb(&serverConn, localMessageSentCb);
}

long TcpSession::getSessionId()
{
    return this->sessionId;
}

bool TcpSession::isSessionValid()
{
    return this->sessionValid;
}

bool TcpSession::registerSessionDisconnectCb(void (*cb)(void *arg, void *obj), void *obj)
{
    this->disconnectedCb = (void (*)(void *, void *))cb;
}

bool TcpSession::registerIncomingMessageCb(void (*cb)(void *arg, char *pdata, unsigned short len, void *obj), void *obj)
{
    this->incomingMessageCb = (void (*)(void *, void *, char *, unsigned short))cb;
}

bool TcpSession::registerMessageSentCb(void (*cb)(void *arg, void *obj), void *obj)
{
    this->messageSentCb = (void (*)(void *, void *))cb;
}


/*******************************************************************************
 * Class Implemenation - private
 *******************************************************************************/

