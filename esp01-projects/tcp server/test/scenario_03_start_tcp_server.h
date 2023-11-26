#include <functional>
#include <doctest.h>

#include <tcp_server.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_regist_cb.h"
#include "mock_espconn_functions.h"

SCENARIO("TCP Server register and handle the connect callback")
{
    espconnRegistConnectCbTestIndex = 0; // Successful registration of connect CB
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
            espconnRegistConnectCbTestIndex = 0; // Successful registration of connect CB
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