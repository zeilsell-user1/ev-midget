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
#include <cstdio>

#ifndef MAX_TCP_SESSIONS
#define MAX_TCP_SESSIONS 10
#endif

#define TCP_LIB_DEBUG 1
#ifdef TCP_LIB_DEBUG
#define TCP_INFO(format, ...)      \
    printf("TCP_INFO: ");          \
    printf(format, ##__VA_ARGS__); \
    printf("\n")
#define TCP_WARNING(format, ...)   \
    printf("TCP_WARNING: ");          \
    printf(format, ##__VA_ARGS__); \
    printf("\n")
#define TCP_ERROR(format, ...)     \
    printf("TCP_ERROR: ");          \
    printf(format, ##__VA_ARGS__); \
    printf("\n")
#else
#define TCP_INFO(format, ...)
#define TCP_WARNING(format, ...)
#define TCP_ERROR(format, ...)
#endif
