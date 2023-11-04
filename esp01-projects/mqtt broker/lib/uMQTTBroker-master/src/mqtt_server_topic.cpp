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

/**
 * @file
 * Topic handling functions.
 *
 * Topic syntax matches that of other MQTT servers such as Micro broker.
 */

#include "mqtt_server_topic.h"

#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "string.h"

MqttServerTopic::MqttServerTopic()
{
	resetTopic();
}

MqttServerTopic::MqttServerTopic(char *topic)
{
	int length = strnlen(topic, MAX_TOPIC_LENGTH);

	if ((length == 0) || (length == MAX_TOPIC_LENGTH))
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

bool MqttServerTopic::setTopic(char *topic)
{
	this->length = strnlen(topic, MAX_TOPIC_LENGTH);

	if ((this->length == 0) || (this->length == MAX_TOPIC_LENGTH))
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

    if (*this->topic == '/')
	{
		return false;
	}

	char *hashpos = strchr(this->topic, '#'); 

	if ((hashpos != NULL) && (hashpos != this->topic + this->length - 1) && (*(hashpos - 1) != '/'))
	{
		return false;
	}

	char *pluspos;
	
	while (pluspos = strchr(this->topic, '+'))
	{
		if ((*(pluspos - 1) != '/') && (*(pluspos + 1) != '/'))
		{
			return false;
		}
	};

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
		return (os_strcmp(this->topic, other.topic) == 0);
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


#ifdef UNIT_TEST

char* MqttServerTopics::_strdup(char *src)
{
	char *str;
	char *p;
	int len = 0;

	while (src[len])
		len++;
	str = (char *)os_malloc(len + 1);
	p = str;
	while (*src)
		*p++ = *src++;
	*p = '\0';
	return str;
}

#if !defined(ARRAY_SIZE)
/**
 * Macro to calculate the number of entries in an array
 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

int test()
{
	int i;

	struct
	{
		char *str;
	} tests0[] = {
		"#", "jj", "+/a", "adkj/a", "+/a", "adsjk/adakjd/a", "a/+", "a/#", "#/a"};

	for (i = 0; i < sizeof(tests0) / sizeof(char *); ++i)
	{
		os_printf("topic %s, isValidName %d\n", tests0[i].str, Topics_isValidName(tests0[i].str));
		// assert(Topics_isValidName(tests0[i].str) == 1);
	}

	struct
	{
		char *wild;
		char *topic;
		int result;
	} tests1[] = {
		{"#", "jj", 1},
		{"+/a", "adkj/a", 1},
		{"+/a", "adsjk/adakjd/a", 0},
		{"+/+/a", "adsjk/adakjd/a", 1},
		{"#/a", "adsjk/adakjd/a", 1},
		{"test/#", "test/1", 1},
		{"test/+", "test/1", 1},
		{"+", "test1", 1},
		{"+", "test1/k", 0},
		{"+", "/test1/k", 0},
		{"/+", "test1/k", 0},
		{"+", "/jkj", 0},
		{"/+", "/test1", 1},
		{"+/+", "/test1", 0},
		{"+/+", "test1/k", 1},
		{"/#", "/test1/k", 1},
		{"/#", "test1/k", 0},
	};

	for (i = 0; i < ARRAY_SIZE(tests1); ++i)
	{
		os_printf("wild: %s, topic %s, result %d %d (should)\n", tests1[i].wild, tests1[i].topic,
				  Topics_matches(_strdup(tests1[i].wild), 1, _strdup(tests1[i].topic)), tests1[i].result);
		// assert(Topics_matches(_strdup(tests1[i].wild), _strdup(tests1[i].topic)) == tests1[i].result);
	}

	struct
	{
		char *str;
		char *result;
	} tests2[] = {
		{"#", "#"}, {"ab", "ba"}, {"abc", "cba"}, {"abcd", "dcba"}, {"abcde", "edcba"}};
	for (i = 0; i < 5; ++i)
	{
		os_printf("str: %s, _strrev %s\n", tests2[i].str, _strrev(_strdup(tests2[i].str)));
		// assert(strcmp(tests2[i].result, _strrev(_strdup(tests2[i].str))) == 0);
	}
}

#endif
