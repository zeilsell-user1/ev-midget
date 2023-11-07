/*******************************************************************************
 * Copyright (c) 2023 George Consultant Ltd.
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
#include "mqtt_server_topic.h"

MqttServerTopic::MqttServerTopic()
{
	resetTopic();
}

MqttServerTopic::MqttServerTopic(char const *topic)
{
	this->length = strnlen(topic, MAX_TOPIC_LENGTH);

	if ((this->length != 0) && (this->length != MAX_TOPIC_LENGTH))
	{
		strcpy(this->topic, topic);

		if (!this->isValidName()) 
		{
			resetTopic();
		}
	}
	else
	{
		resetTopic();
	}
}

/**
 * sets the topic
 * @return returns false if there is a proboem with the topic string
 */

bool MqttServerTopic::setTopic(char const *topic)
{
	this->length = strnlen(topic, MAX_TOPIC_LENGTH);

	if ((this->length != 0) && (this->length != MAX_TOPIC_LENGTH))
	{
		strcpy(this->topic, topic);

		if (!this->isValidName()) 
		{
			resetTopic();
			return false;
		}
	}
	else
	{
		resetTopic();
		return false;
	}
	return true;
}

/**
 * resets the topic to all '\0'
 */

void MqttServerTopic::resetTopic()
{
	memset(this->topic, '\0', MAX_TOPIC_LENGTH);
	this->length = 0;
}

/**
 * @brief get the length of the topic
 */

unsigned char MqttServerTopic::getLength()
{
	return this->length;
}

/**
 * Checks that the syntax of a topic string is correct.
 * @return boolean value indicating whether the topic name is valid
 */

bool MqttServerTopic::isValidName() const
{
	// Topics are checked for length when instantiated so no need to check again
	//
	// Topics can't start with a'/'
	//
	// wildcards are either '+' or '#'. 
	// '#" is a legal wildcard character, representing a multi-level wildcard that matches 
    //     any number of levels in the topic hierarchy.
    // "+" is also a legal wildcard character, representing a single level wildcard that 
	//     matches exactly one level in the topic hierarchy.
    // If '#' is used, it must be the last character in the topic string.
    // If '#' is not in the first position, it must be preceded by a '/' character.
    // If '+' is used, it must be preceded by a '/' character.
    // A '+' wildcard must always be followed by a '/' character.
	//
	// A topic can start with a '$' 

    if (*(this->topic) == '\0')
	{
		MQTT_ERROR("empty string");
		return false;
	}
	
    if (*(this->topic) == '$')
	{
		return true;
	}
	
    if (*(this->topic) == '#')
	{
		if (*(this->topic + 1) == '\0')
		{
			return true; // only character
		}
		else
		{
			return false; // no following characters allowed
		}
	}
	
    if (*(this->topic + this->length - 1) == '#') // minus the '\0'
	{
		if (*(this->topic + this->length - 2) == '/') // minus the '#' and '\0'
		{
			return true; // last character is # and preceeded by a /
		}
		else
		{
			return false; // something other than / before the #
		}
	}
	
    if (*(this->topic) == '/')
	{
		MQTT_ERROR("topic started with a '/'");
		return false;
	}

	if (strchr(this->topic, '#') != NULL) 
	{
		MQTT_ERROR("failed with # not at start or end");
		return false;
	}

	char *pluspos = strchr(this->topic, '+');
	
	while (pluspos != nullptr)
	{
		if ((*(pluspos - 1) != '/') && (*(pluspos + 1) != '/'))
		{
			MQTT_ERROR("failed checking the '+' locations");
			return false;
		}

		pluspos = strchr(this->topic, '+');
	};

	MQTT_INFO("topic is valid");
	return true;
}

/**
 * Does a topic string contain wildcards?
 * @return boolean value indicating whether the topic contains a wildcard or not
 */

bool MqttServerTopic::hasWildcards() const
{
	return (strchr(this->topic, '+') != NULL) || (strchr(this->topic, '#') != NULL);
}

/**
 * Tests whether one topic string matches another where one can contain wildcards.
 * 
 * Single Level Wildcard (+):
 * The + wildcard matches a single level in the topic hierarchy.
 * For example, if a subscriber subscribes to sensors/+/temperature, it will receive 
 * messages published to topics like sensors/room1/temperature or sensors/room2/temperature, 
 * but not sensors/room1/humidity.
 * 
 * Multi Level Wildcard (#):
 * 
 * The # wildcard matches all remaining levels in the topic hierarchy.
 * It must be the last character in the subscribed topic.
 * For example, if a subscriber subscribes to sensors/#, it will receive messages published 
 * to topics like sensors/room1/temperature, sensors/room2/humidity, and even just sensors 
 * (the root level).
 * 
 * https://www.hivemq.com/blog/mqtt-essentials-part-5-mqtt-topics-best-practices/
 * 
 * @param other a topic string should be matched to this topic string
 * @return boolean value indicating whether topic matches wildTopic
 */

bool MqttServerTopic::operator==(MqttServerTopic& other)
{
	bool thisHasWildcards = true;
	bool thatHasWildcards = true;

	// only one of the topics may have a wildcard, so check for wildcards on both

	if ((thisHasWildcards = this->hasWildcards()) && (thatHasWildcards = other.hasWildcards())) {
		return false;
	}

	// if neither have wildcards then straight compare

	if (!thisHasWildcards && !thisHasWildcards)
	{
		return (strcmp(this->topic, other.topic) == 0);
	}

	// if the either of the topics are just a '#' then this is a match to anything except if
	// the other topic starts with a '$' (special system command)

	if ((*(this->topic) == '#') || (*(other.topic) == '#'))
	{
		if ((*(this->topic) == '$') || (*(other.topic) == '$'))
			return false;
		return true;
	}

	// loop through the tokens, comparing each. If they differ then we can return false
	// unless we find a wildcard. If either token is a '#' then we match the rest of the
	// topic. If the match a '+' then the token check gets matched regardless

	char *topicToken, *topicTokenSave;
	topicToken = strtok_r(this->topic, "/", &topicTokenSave);

	char *otherTopicToken, * otherTopicTokenSave;
	otherTopicToken = strtok_r(other.topic, "/", &otherTopicTokenSave);

	bool match = true;

	while ( (topicToken != NULL) && (otherTopicToken != NULL) && 
	        (*topicToken != '#') && (*otherTopicToken != '#') &&
		    (match == true) )
	{
		// both tokens are valid. If they match then skip to next token

		if (strcmp(topicToken, otherTopicTokenSave) != 0)
		{
			// need to check if this is a '+', as this implies a match

			if ((*topicToken != '+') && (*otherTopicToken != '+'))
			{
				match = false; 
			}
		}
	}

    return match;
} /* end matches */

/*****************************************************************************
 * Private methods
******************************************************************************/


