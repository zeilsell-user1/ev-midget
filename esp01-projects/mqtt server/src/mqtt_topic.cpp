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
#include "mqtt_topic.h"

MqttTopic::MqttTopic()
{
	resetTopic();
}

MqttTopic::MqttTopic(char const *topic)
{
	length_ = strnlen(topic, MAX_TOPIC_LENGTH);

	if ((length_ != 0) && (length_ != MAX_TOPIC_LENGTH))
	{
		strcpy(topic_, topic);

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

bool MqttTopic::setTopic(char const *topic)
{
	length_ = strnlen(topic, MAX_TOPIC_LENGTH);

	if ((length_ != 0) && (length_ != MAX_TOPIC_LENGTH))
	{
		strcpy(topic_, topic);

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

void MqttTopic::resetTopic()
{
	memset(topic_, '\0', MAX_TOPIC_LENGTH);
	length_ = 0;
}

/**
 * @brief get the length of the topic
 */

unsigned char MqttTopic::getLength()
{
	return length_;
}

/**
 * Checks that the syntax of a topic string is correct.
 * @return boolean value indicating whether the topic name is valid
 */

bool MqttTopic::isValidName() const
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
	// Only one '#' or '+' is allowed in a topic
	// Although a leading '/' is allowed, in this implemenation it is rejected as bad practice
	// Although ' ' is allowed, in this implementation it is rejected as bad practice
	//
	// A topic can start with a '$' and all that follows is free text

    if (*(topic_) == '\0')
	{
		MQTT_ERROR("empty string");
		return false;
	}
	
    if (*(topic_) == '/')
	{
		MQTT_ERROR("topic started with a '/'");
		return false;
	}

    if (*(topic_) == '$')
	{
		MQTT_INFO("topic started with $");
		return true;
	}
	
	if (strchr(topic_, ' ') != NULL)
	{
		MQTT_ERROR("space character found in topic");
		return false;
	}

	if (strstr(topic_, "//") != NULL)
	{
		MQTT_ERROR("'//' found in topic");
		return false;
	}

	if ((strchr(topic_, '#') != NULL) && (strchr(topic_, '+') != NULL))
	{
		MQTT_ERROR("failed with # and +");
		return false;
	}
	
	if (numberOfOccurences((const char *)(topic_), (const char)'#') > 1)
	{
		MQTT_ERROR("more than 1 '#' in topic");
		return false;
	}
	
	if (numberOfOccurences((const char *)(topic_), (const char)'+') > 1)
	{
		MQTT_ERROR("more than 1 '+' in topic");
		return false;
	}

    if (*(topic_) == '#')
	{
		if (*(topic_ + 1) == '\0')
		{
		    MQTT_INFO("just a '#' in the topic");
			return true;
		}
		else
		{
			MQTT_ERROR("a '#' at the start can't be followed by anything");
			return false; 
		}
	}

    if (*(topic_ + length_ - 1) == '#') // minus the '\0'
	{
		if (*(topic_ + length_ - 2) == '/') // minus the '#' and '\0'
		{
		    MQTT_INFO("topic ends with a '/#' so okay");
			return true; 
		}
		else
		{
			MQTT_ERROR("The '#' at the end isn't preceeded by a '/'");
			return false; 
		}
	}
	
	if (strchr(topic_, '#') != NULL) 
	{
		MQTT_ERROR("failed with # as checks for start or end already completed");
		return false;
	}

	char *pluspos = strchr(topic_, '+');
	
	if ((pluspos != NULL) && ((*(pluspos - 1) != '/')  || (*(pluspos + 1) != '/')))
	{
		MQTT_ERROR("fthe '+' didn't have '/' either side");
		return false;
	}

	MQTT_INFO("topic is valid");
	return true;
}

/**
 * Does a topic string contain wildcards?
 * @return boolean value indicating whether the topic contains a wildcard or not
 */

bool MqttTopic::hasWildcards() const
{
	return (strchr(topic_, '+') != NULL) || (strchr(topic_, '#') != NULL);
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

bool MqttTopic::operator==(MqttTopic& other)
{
	bool thisHasWildcards = this->hasWildcards();
	bool thatHasWildcards = other.hasWildcards();

    MQTT_INFO("this topic = %s", topic_);
	MQTT_INFO("that topic = %s", other.topic_);
	MQTT_INFO("thisHasWildcards %i", thisHasWildcards);
	MQTT_INFO("thatHasWildcards %i", thatHasWildcards);

	// only one of the topics may have a wildcard, so check for wildcards on both

	if ((thisHasWildcards && thatHasWildcards)) {
		MQTT_INFO("Wildcards in both topics so no match");
		return false;
	}

	// if neither have wildcards then straight compare

	if (!thisHasWildcards && !thatHasWildcards)
	{
		MQTT_INFO("no wildcards so straight compare");
		return (strcmp(topic_, other.topic_) == 0);
	}

	// if the either of the topics are just a '#' then this is a match to anything except if
	// the other topic starts with a '$' (special system command)

	if ((*(topic_) == '#') || (*(other.topic_) == '#'))
	{
		if ((*(topic_) == '$') || (*(other.topic_) == '$'))
		{
			MQTT_INFO("dont match # and $");
			return false;
		}
		MQTT_INFO("# wildcard matches all");
		return true;
	}

	// loop through the tokens, comparing each. If they differ then we can return false
	// unless we find a wildcard. If either token is a '#' then we match the rest of the
	// topic. If the match a '+' then the token check gets matched regardless

	char *topicToken, topicCopy[MAX_TOPIC_LENGTH], *topicTokenSave;
	char *otherTopicToken, otherTopicCopy[MAX_TOPIC_LENGTH], *otherTopicTokenSave;

	memset(topicCopy, '\0', MAX_TOPIC_LENGTH);
	memset(otherTopicCopy, '\0', MAX_TOPIC_LENGTH);

	strcpy(topicCopy, topic_);
	strcpy(otherTopicCopy, other.topic_);

	MQTT_INFO("copy this %s, copy other %s", topicCopy, otherTopicCopy);

	topicToken = strtok_r(topicCopy, "/", &topicTokenSave);
	otherTopicToken = strtok_r(otherTopicCopy, "/", &otherTopicTokenSave);

	MQTT_INFO("time to compare tokens - %s, %s", topicToken, otherTopicToken);
	
	bool match = true;

	while ( (topicToken != NULL) && (otherTopicToken != NULL) && 
	        (*topicToken != '#') && (*otherTopicToken != '#') &&
		    (match == true) )
	{
		// both tokens are valid. If they match then skip to next token

		if (strcmp(topicToken, otherTopicToken) != 0)
		{
			MQTT_INFO("the tokens don't match");
			// need to check if this is a '+', as this implies a match

			if ((*topicToken != '+') && (*otherTopicToken != '+'))
			{
				MQTT_INFO("so return a false match");
				match = false; 
			}
			else
			{
				MQTT_INFO("but a + was found so keep match true");
				match = true;
			}
		}
		topicToken = strtok_r(topicTokenSave, "/", &topicTokenSave);
		otherTopicToken = strtok_r(otherTopicTokenSave, "/", &otherTopicTokenSave);
		MQTT_INFO("next tokens to compare - %s, %s", topicToken, otherTopicToken);
	}

	MQTT_INFO("does this topic %s = that topic %s?", topic_, other.topic_);
    MQTT_INFO("return %i", match);
    return match;
} /* end matches */

/*****************************************************************************
 * Private methods
******************************************************************************/

unsigned char MqttTopic::numberOfOccurences(const char *str, const char chr) const
{
	unsigned char count = 0;

  	for (int i = 0; i < strnlen(str, MAX_TOPIC_LENGTH) && (i < 127); i++)
	{
		if (str[i] == chr) count++;
	}

  	return count;
}