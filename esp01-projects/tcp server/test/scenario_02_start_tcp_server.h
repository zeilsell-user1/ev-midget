
#include <functional>
#include <doctest.h>

#include <tcp_server.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_regist_cb.h"
#include "mock_espconn_functions.h"

SCENARIO("TCP Server being started")
{
    GIVEN("that the TcpServer has been retrieved with a getInstance")
    {
        unsigned short port = TEST_PORT_1;

        class MockOwner
        {
        public:
            MockOwner() {}

            void mockServerConnectedCb(void *ownerObj, TcpSession::TcpSessionPtr Session)
            {
                FAIL("In this test, the mockServerConnectedCb should not be called");
            }
        } mockOwner; // Instantiate an object of the local class

        // Use a lambda function as the callback when ESPCONN connects to a session
        auto callback = [](void *ownerObj, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockServerConnectedCb(ownerObj, session);
        };

        WHEN("get a startTcpServer for the first time")
        {
            TcpServer &tcpServer = TcpServer::getInstance();
            tcpServer.cleanup();  // ensure the server is clean from the start
            espconnRegistConnectCbTestIndex = 0; // successful registration of connect CB

            THEN("the result of calling startTcpServer is true")
            {
                bool result = tcpServer.startTcpServer(port, callback, &mockOwner);
                REQUIRE_EQ(result, true);
            }
        }
        WHEN("startTcpServer is called for a second time")
        {
            TcpServer &tcpServer = TcpServer::getInstance();
            espconnRegistConnectCbTestIndex = 0; // successful registration of connect CB

            THEN("the result of calling startTcpServer a second time is false")
            {
                bool result = tcpServer.startTcpServer(port, callback, &mockOwner);
                REQUIRE_EQ(result, false);
            }
        }
        WHEN("error in the registration of the callback")
        {
            TcpServer &tcpServer = TcpServer::getInstance();
            tcpServer.cleanup();  // ensure the server is clean from the start
            espconnRegistConnectCbTestIndex = 1; // unsuccessful registration of connect CB

            THEN("the result of calling startTcpServer is false")
            {
                bool result = tcpServer.startTcpServer(port, callback, &mockOwner);
                REQUIRE_EQ(result, false);
            }
        }
    }
}