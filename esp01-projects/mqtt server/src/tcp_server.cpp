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
#include "tcp_server.h"

/*******************************************************************************
 * local null callback used to make sure any bugs are handled correctly. THis
 * is outside of the class so that the callbacks can work across classes
*******************************************************************************/

void nullCallback1(TcpSession *tcpSession)
{
    TCP_ERROR("callback called without initialisation");
}

/*******************************************************************************
 * Class Implemenation - public
*******************************************************************************/

TcpServer::TcpServer()
{
    ip4_addr_set_any(&this->ipAddress);
    this->port = 0;
    this->connectedCb = nullCallback1;
}

TcpServer::TcpServer(ip_addr_t ipAddress, unsigned short port) 
{
    this->ipAddress = ipAddress;
    this->port = port;
    this->connectedCb = nullCallback1;
}

TcpServer::TcpServer(unsigned short port)
{
    ip4_addr_set_any(&this->ipAddress);
    this->port = port;
    this->connectedCb = nullCallback1;
}

bool TcpServer::registerSessionConnect_cb(void (*cb)(void*, TcpSession*), void *obj)
{
    this->connectedCb = (void (*)(TcpSession *tcpSession))cb;
}

/*******************************************************************************
 * Class Implemenation - private
*******************************************************************************/
