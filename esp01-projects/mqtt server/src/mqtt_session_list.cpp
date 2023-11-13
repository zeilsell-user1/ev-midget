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
#include "mqtt_session_list.h"

MqttSessionList::MqttSessionList()
{
    for (int i; i < MAX_SESSIONS; i++)
    {
        this->list[i].setSessionFalse();
    }
}

bool MqttSessionList::isFull()
{
    bool sessionValid;
    int i = 0;

    {
        sessionValid = this->list[i].isSessionValid();

    } while ((sessionValid == true) && (i < MAX_SESSIONS));

    return sessionValid;
}

void MqttSessionList::addSession(TcpSession *tcpSession)
{
    bool sessionValid;
    int i = 0;

    {
        sessionValid = this->list[i].isSessionValid();

    } while ((sessionValid == true) && (i < MAX_SESSIONS));

    if (sessionValid == false) // the next open session
    {
        this->list[i] = *tcpSession;
    }
}