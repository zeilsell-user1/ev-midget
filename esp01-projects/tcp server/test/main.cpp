#define DOCTEST_CONFIG_IMPLEMENT // REQUIRED: Enable custom main()
#define DOCTEST_THREAD_LOCAL

#include <functional>

#include <doctest.h>
#include <tcp_server.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_regist_cb.h"
#include "mock_espconn_functions.h"

// Test cases implemented in this collection:
//
// Instantiation of the server:
// 1: Check if the server is correctly instantiated as a singleton and
//    verify that the server starts successfully.
//
// Adding and Removing Sessions:
// 2: Add a session and ensure it's present in the server's session list.
// 3: Remove a session and verify that it's no longer in the list.
// 4: Attempt to remove a non-existent session and handle the case appropriately.
//
// Session Callbacks:
// 5: Register various callback functions for a session and ensure they are properly invoked.
// 6: Test the session disconnect callback when a session is disconnected.
//
// Handling Connections:
// 7: Test the server's ability to accept incoming connections.
// 8:Verify that the server handles client disconnections appropriately.
//
// Session Lookup:
// 9: Retrieve a session using its ID and ensure it's the correct session.
// 10: Attempt to retrieve a non-existent session and handle the case appropriately.
//
// Multiple Sessions:
// 11: Add multiple sessions to the server and ensure they are all tracked correctly.
// 12: Test the server's behavior with a high number of concurrent sessions.
//
// Error Handling:
// 13: Test the server's behavior when encountering errors during operations.
// 14: Check how the server handles unexpected events, such as receiving data without a corresponding session.
//
// Client Connection:
// 15: Test the server's ability to initiate connections to other servers.
// 16: Verify that the server can handle disconnections in this scenario.
//
// Callback Ownership:
// 17: Ensure that the callbacks retain the correct ownership information (i.e., the upper layer object).
//
// Session Expiry:
// 18: Test the behavior of sessions with different expiry intervals.
// 19: Check if sessions are correctly expired after reaching their timeout.
//
// IPv6 Support:
// 20: Check it is not supported ATM
//
// Edge Cases:
// 21: Zero Port: Test the server's behavior when starting with port number 0.
// 22: Maximum Port Number: Use the highest possible port number and verify the server's response.
// 23: Invalid IP Address: Attempt to start the server with an invalid IP address and check how it handles the error.
// 24: Null Callbacks: Register null callbacks and ensure the server handles them appropriately.
// 25: Zero Expiry Interval: Set the expiry interval of a session to zero and verify its behavior.
// 26: Negative Expiry Interval: Test how the server handles sessions with negative expiry intervals.
// 27: Overflow Session ID: Test the server's behavior when the session ID reaches its maximum value and overflows.
// 28: Invalid Session Lookup: Attempt to retrieve a session with an invalid (out-of-bounds) session ID.
// 29: Extreme Number of Sessions: Add a very large number of sessions to the server and assess its performance and resource usage.
// 30: Multiple Callback Registrations: Register the same callback multiple times and ensure it doesn't result in issues.
// 31: Unexpected Disconnects: Simulate unexpected disconnects during various stages of a session's lifecycle.
// 32: Unusual Protocol States: Place a session in a protocol state that it shouldn't be in and check how the server reacts.
// 33: Invalid Initialization Order: Test the behavior of the server when methods are called in an unexpected order.
// 34: IPv4/IPv6 Compatibility: If applicable, test the server's behavior when handling both IPv4 and IPv6 connections.
// 35: Resource Exhaustion: Stress test the server by attempting to create sessions beyond the available resources.
// 36: Network Partition: Simulate network partitions and reconnects to evaluate the server's resilience.
// 37: Session Lookup Race Conditions: Introduce race conditions in session lookup and verify the server's thread safety.
// 38: Mixed Session Types: Mix different types of sessions (e.g., server-initiated and client-initiated) and observe the server's behavior.
// 39: Repeated Server Start/Stop: Start and stop the server multiple times in rapid succession and ensure it remains stable.
// 40: Unusual Characters in IP Address: Include non-numeric characters in the IP address and verify how the server handles them.

SCENARIO("TCP Server can be instantiated")
{
    espconnRegistConnectCbTestIndex = 0;
    TcpServer &tcpServer = TcpServer::getInstance();

    GIVEN("that TcpServer is a singleton")
    {
        WHEN("getInstance is called twice")
        {
            TcpServer &tcpServer2 = TcpServer::getInstance();

            THEN("the instance is the same and the session count is zero")
            REQUIRE_EQ(&tcpServer, &tcpServer2);
            REQUIRE_EQ(tcpServer.getSessionCount(), 0);
        }
    }
    GIVEN("that the TcpServer has been retrieved with a getInstance")
    {
        // espconn mockedEspconn;
        unsigned short port = TEST_PORT_1;

        class MockOwner
        {
        public:
            MockOwner() {}

            void mockServerConnectedCb(void *ownerObj, TcpSession::TcpSessionPtr Session)
            {
                INFO("The callback that is called by TCP Server if a session is connected");
                FAIL("In this test, the callback should not be called");
            }
        } mockOwner; // Instantiate an object of the local class

        // Use a lambda function as the callback when ESPCONN connects to a session
        auto callback = [](void *ownerObj, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockServerConnectedCb(ownerObj, session);
        };

        WHEN("get a startTcpServer for the first time")
        {
            THEN("the result of calling startTcpServer is true")
            {
                bool result = tcpServer.startTcpServer(port, callback, &mockOwner);
                REQUIRE_EQ(result, true);
            }
        }

        WHEN("startTcpServer is called for a second time")
        {
            THEN("the result of calling startTcpServer a secomd time is false")
            {
                bool result = tcpServer.startTcpServer(port, callback, &mockOwner);
                REQUIRE_EQ(result, false);
            }
        }
    }
}

SCENARIO("TCP Server can be started and handles the connect callback")
{
    espconnRegistConnectCbTestIndex = 0;
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.cleanup();

    GIVEN("that the TcpServer has been retrieved with a getInstance and started as a server")
    {
        espconn mockedEspconn;
        unsigned short port = TEST_PORT_1;

        class MockOwner
        {
        public:
            MockOwner() {}
            bool callbackCalled = false;

            void mockServerConnectedCb(void *ownerObj, TcpSession::TcpSessionPtr session)
            {
                INFO("mockServerConnectedCb should be called in this test");
                REQUIRE_EQ(session->isSessionValid(), true);
                callbackCalled = true;
            }
        } mockOwner; // Instantiate an object of the local class

        // Use a lambda function as the callback when ESPCONN connects to a session
        auto callback = [](void *ownerObj, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockServerConnectedCb(ownerObj, session);
        };

        bool result = tcpServer.startTcpServer(port, callback, &mockOwner);
        REQUIRE_EQ(result, true);

        WHEN("when the connected callback is called")
        {
            esp_tcp tcp;
            mockedEspconn.type = ESPCONN_TCP;
            mockedEspconn.state = ESPCONN_CONNECT;
            tcp.remote_ip[0] = IP_1;
            tcp.remote_ip[1] = IP_2;
            tcp.remote_ip[2] = IP_3;
            tcp.remote_ip[3] = IP_4;
            tcp.remote_port = TEST_PORT_1;
            mockedEspconn.proto.tcp = &tcp;

            connectCb_(&mockedEspconn);

            REQUIRE_EQ(mockOwner.callbackCalled, true);
        }
    }
}

SCENARIO("TCP Server can be started and can register all other callbacks")
{
    espconnRegistConnectCbTestIndex = 0;
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.cleanup();

    GIVEN("that the TcpServer has been retrieved with a getInstance and started as a server")
    {
        espconn mockedEspconn;
        unsigned short port = TEST_PORT_1;

        class MockOwner
        {
        public:
            void (*disconnectedCb_)(void *, TcpSession::TcpSessionPtr);
            void (*incomingMessageCb_)(void *, char *, unsigned short, TcpSession::TcpSessionPtr);
            void (*sentCb_)(void *, TcpSession::TcpSessionPtr);

            MockOwner() {}
            MockOwner(void (*disconnectedCb)(void *, TcpSession::TcpSessionPtr),
                      void (*incomingMessageCb)(void *, char *, unsigned short, TcpSession::TcpSessionPtr),
                      void (*sentCb)(void *, TcpSession::TcpSessionPtr))
            {
                disconnectedCb_ = disconnectedCb;
                incomingMessageCb_ = incomingMessageCb;
                sentCb_ = sentCb;
            }

            bool callbackCalled = false;

            void mockServerConnectedCb(void *ownerObj, TcpSession::TcpSessionPtr session)
            {
                TcpServer &tcpServer = TcpServer::getInstance();
                INFO("mockServerConnectedCb should be called in this test");
                REQUIRE_EQ(session->isSessionValid(), true);
                callbackCalled = true;

                session->registerIncomingMessageCb(incomingMessageCb_);
                session->registerMessageSentCb(sentCb_);
                session->registerSessionDisconnectCb(disconnectedCb_);
            }

            void mockDisconnectedCb(void *ownerObj, TcpSession::TcpSessionPtr session)
            {
                TcpServer &tcpServer = TcpServer::getInstance();
                INFO("mockServerConnectedCb should be called in this test");
                REQUIRE_EQ(session->isSessionValid(), true);
            }

            void mockIncomingMessageCb(void *ownerObj, char *pdata, unsigned short len, TcpSession::TcpSessionPtr session)
            {
                TcpServer &tcpServer = TcpServer::getInstance();
                INFO("mockServerConnectedCb should be called in this test");
                REQUIRE_EQ(session->isSessionValid(), true);
            }

            void mockMessageSentCb(void *ownerObj, TcpSession::TcpSessionPtr session)
            {
                TcpServer &tcpServer = TcpServer::getInstance();
                INFO("mockServerConnectedCb should be called in this test");
                REQUIRE_EQ(session->isSessionValid(), true);
            }
        }; // mockOwner; // Instantiate an object of the local class

        // Use a lambda function as the callback when ESPCONN connects to a session
        auto connectedCb = [](void *ownerObj, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockServerConnectedCb(ownerObj, session);
        };

        // Use a lambda function as the callback when ESPCONN connects to a session
        auto disconnectedCb = [](void *ownerObj, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockDisconnectedCb(ownerObj, session);
        };

        // Use a lambda function as the callback when ESPCONN connects to a session
        auto incomingMessageCb = [](void *ownerObj, char *pdata, unsigned short len, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockIncomingMessageCb(ownerObj, pdata, len, session);
        };

        // Use a lambda function as the callback when ESPCONN connects to a session
        auto sentCb = [](void *ownerObj, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockMessageSentCb(ownerObj, session);
        };

        MockOwner mockOwner = MockOwner(disconnectedCb, incomingMessageCb, sentCb);

        bool result = tcpServer.startTcpServer(port, connectedCb, &mockOwner);
        REQUIRE_EQ(result, true);

        WHEN("when the connected callback is called")
        {
            esp_tcp tcp;
            mockedEspconn.type = ESPCONN_TCP;
            mockedEspconn.state = ESPCONN_CONNECT;
            tcp.remote_ip[0] = IP_1;
            tcp.remote_ip[1] = IP_2;
            tcp.remote_ip[2] = IP_3;
            tcp.remote_ip[3] = IP_4;
            tcp.remote_port = TEST_PORT_1;
            mockedEspconn.proto.tcp = &tcp;

            connectCb_(&mockedEspconn);

            REQUIRE_EQ(mockOwner.callbackCalled, true);
        }
    }
}

int main(int argc, char **argv)
{
    doctest::Context context;

    // BEGIN:: PLATFORMIO REQUIRED OPTIONS
    context.setOption("success", true);     // Report successful tests
    context.setOption("no-exitcode", true); // Do not return non-zero code on failed test case
    // END:: PLATFORMIO REQUIRED OPTIONS

    // YOUR CUSTOM DOCTEST OPTIONS
    context.applyCommandLine(argc, argv);
    return context.run();
}
