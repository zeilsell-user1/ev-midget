
#include <functional>
#include <doctest.h>

#include <tcp_server.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_regist_cb.h"
#include "mock_espconn_functions.h"
#include "mock_owner_cb_functions.h"
#include "mock_utils.h"

SCENARIO("TCP Server being started")
{
    GIVEN("that the TcpServer has been retrieved with a getInstance")
    {
        unsigned short port = TEST_PORT_1;
        ip_addr_t ipAddress;
        IP4_ADDR(&ipAddress, IP_1, IP_2, IP_3, IP_4);

        WHEN("get a startTcpServer for the first time")
        {
            TcpServer &tcpServer = TcpServer::getInstance();
            tcpServer.cleanup();                 // ensure the server is clean from the start
            espconnRegistConnectCbTestIndex = 0; // successful registration of connect CB
            espconnRegistConnectCbCalled = false;

            THEN("the result of calling startTcpServer is true")
            {
                REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);
                REQUIRE_EQ(espconnRegistConnectCbCalled, true);
            }
        }
        WHEN("startTcpServer is called for a second time")
        {
            TcpServer &tcpServer = TcpServer::getInstance();
            espconnRegistConnectCbTestIndex = 0; // successful registration of connect CB
            espconnRegistConnectCbCalled = false;

            THEN("the result of calling startTcpServer a second time is false")
            {
                REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), false);
                REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            }
        }
        WHEN("startTcpClient is called")
        {
            TcpServer &tcpServer = TcpServer::getInstance();
            espconnRegistConnectCbTestIndex = 0; // successful registration of connect CB
            espconnRegistConnectCbCalled = false;

            THEN("the result of calling startTcpClient is false")
            {
                REQUIRE_EQ(tcpServer.startTcpClient(ipAddress, port, connectedCb, &dummyObject), false);
                REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            }
        }
        WHEN("error in the registration of the callback")
        {
            TcpServer &tcpServer = TcpServer::getInstance();
            tcpServer.cleanup();                 // ensure the server is clean from the start
            espconnRegistConnectCbTestIndex = 1; // unsuccessful registration of connect CB
            espconnRegistConnectCbCalled = false;

            THEN("the result of calling startTcpServer is false")
            {
                REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), false);
                REQUIRE_EQ(espconnRegistConnectCbCalled, true);
            }
        }
    }
}