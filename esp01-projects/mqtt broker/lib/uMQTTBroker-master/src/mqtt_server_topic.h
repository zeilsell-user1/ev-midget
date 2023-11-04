/*******************************************************************************
 * Copyright (c) 2007, 2013 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

/*******************************************************************************
 * Based off the C library covered by the attributation and library above
 * Rewritten by Richard George (richard.john.george.3@gmail.com)
 * Converted to C++ with proper OOP structure
 *******************************************************************************/

#ifndef MQTT_SERVER_TOPIC_H
#define MQTT_SERVER_TOPIC_H

#define TOPIC_LEVEL_SEPARATOR "/"
#define SINGLE_LEVEL_WILDCARD "+"
#define MULTI_LEVEL_WILDCARD  "#"

#include "c_types.h"

class MqttServerTopic
{
    private:
        static const uint8_t MAX_TOPIC_LENGTH = 30;
        char topic[MAX_TOPIC_LENGTH];
        uint8_t length;
        
    public:
    
        MqttServerTopic();
        MqttServerTopic(char *topic);
        bool setTopic(char *topic);
        void resetTopic();
        bool isValidName() const;
        bool hasWildcards() const;
        bool operator==(MqttServerTopic& other);
};

#endif /* MQTT_SERVER_TOPIC_H */
