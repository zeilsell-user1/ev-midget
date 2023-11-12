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

#ifndef MQTT_SESSION_H
#define MQTT_SESSION_H

#include "defaults.h"
//#include "espconn.h"

class MqttSession
{
    private:
        bool sessionValid;
        unsigned char will_qos;
        int will_retain;
        int clean_session;
        //struct espconn *pesp_conn;
        unsigned char clientId[23];
        unsigned char IPAddress[4];
        unsigned long sessionExpiryIntervalTimeout;
            
        void WaitForConnect_HandleMsg(MqttMsg msg);
        void Connected_HandleMsg(MqttMsg msg);
        void WaitForPubRel_HandleMsg(MqttMsg msg);
        void Disconnected_HandleMsg(MqttMsg msg);


    public:
    
        MqttSession();
};

#endif /* MQTT_SESSION_H */