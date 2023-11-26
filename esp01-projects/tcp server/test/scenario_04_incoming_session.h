#include <functional>
#include <doctest.h>

#include <tcp_server.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_regist_cb.h"
#include "mock_espconn_functions.h"

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
            void (*reconnectCb_)(void *, signed char, TcpSession::TcpSessionPtr);

            MockOwner() {}
            MockOwner(void (*disconnectedCb)(void *, TcpSession::TcpSessionPtr),
                      void (*incomingMessageCb)(void *, char *, unsigned short, TcpSession::TcpSessionPtr),
                      void (*sentCb)(void *, TcpSession::TcpSessionPtr),
                      void (*reconnectCb)(void *, signed char, TcpSession::TcpSessionPtr))
            {
                disconnectedCb_ = disconnectedCb;
                incomingMessageCb_ = incomingMessageCb;
                sentCb_ = sentCb;
                reconnectCb_ = reconnectCb;
            }

            bool callbackCalled = false;

            void mockServerConnectedCb(void *ownerObj, TcpSession::TcpSessionPtr session)
            {
                TcpServer &tcpServer = TcpServer::getInstance();
                INFO("mockServerConnectedCb should be called in this test");
                REQUIRE_EQ(session->isSessionValid(), true);
                REQUIRE_EQ(tcpServer.getSessionCount(), 1);
                callbackCalled = true;

                bool imrc = session->registerIncomingMessageCb(incomingMessageCb_, (void *)this);
                if (espconnRegistRecvCbTestIndex != 0)
                {
                    REQUIRE_EQ(imrc, false);
                    return;
                }
                else
                {
                    REQUIRE_EQ(imrc, true);
                }

                bool msrc = session->registerMessageSentCb(sentCb_, (void *)this);
                if (espconnRegistSentCbTestIndex != 0)
                {
                    REQUIRE_EQ(msrc, false);
                    return;
                }
                else
                {
                    REQUIRE_EQ(msrc, true);
                }

                bool sdrc = session->registerSessionDisconnectedCb(disconnectedCb_, (void *)this);
                if (espconnRegistDisconnectCbTestIndex != 0)
                {
                    REQUIRE_EQ(sdrc, false);
                    return;
                }
                else
                {
                    REQUIRE_EQ(sdrc, true);
                }

                bool srrc = session->registerSessionReconnectCb(reconnectCb_, (void *)this);

                if (espconnRegistReconnectCbTestIndex != 0)
                {
                    REQUIRE_EQ(srrc, false);
                    return;
                }
                else
                {
                    REQUIRE_EQ(srrc, true);
                }
            }

            void mockDisconnectedCb(void *ownerObj, TcpSession::TcpSessionPtr session)
            {
                TcpServer &tcpServer = TcpServer::getInstance();
                FAIL("mockDiconnectedCb should not be called in this test");
            }

            void mockIncomingMessageCb(void *ownerObj, char *pdata, unsigned short len, TcpSession::TcpSessionPtr session)
            {
                TcpServer &tcpServer = TcpServer::getInstance();
                FAIL("mockIncomingMessageCb should not be called in this test");
            }

            void mockMessageSentCb(void *ownerObj, TcpSession::TcpSessionPtr session)
            {
                TcpServer &tcpServer = TcpServer::getInstance();
                FAIL("mockMessageSentCb should not be called in this test");
            }

            void mockReconnectCb(void *ownerObj, signed char err, TcpSession::TcpSessionPtr session)
            {
                TcpServer &tcpServer = TcpServer::getInstance();
                FAIL("mockReconnectCb should not be called in this test");
            }
        }; // mockOwner; // Instantiate an object of the local class

        // Use a lambda function as the callback to avoid class ownership
        auto connectedCb = [](void *ownerObj, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockServerConnectedCb(ownerObj, session);
        };

        auto disconnectedCb = [](void *ownerObj, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockDisconnectedCb(ownerObj, session);
        };

        auto incomingMessageCb = [](void *ownerObj, char *pdata, unsigned short len, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockIncomingMessageCb(ownerObj, pdata, len, session);
        };

        auto sentCb = [](void *ownerObj, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockMessageSentCb(ownerObj, session);
        };

        auto reconnectCb = [](void *ownerObj, signed char err, TcpSession::TcpSessionPtr session)
        {
            static_cast<MockOwner *>(ownerObj)->mockReconnectCb(ownerObj, err, session);
        };

        MockOwner mockOwner = MockOwner(disconnectedCb, incomingMessageCb, sentCb, reconnectCb);

        bool result = tcpServer.startTcpServer(port, connectedCb, &mockOwner);
        REQUIRE_EQ(result, true);

        WHEN("when the connected callback is called successfully")
        {
            espconnRegistSentCbTestIndex = 0;
            espconnRegistRecvCbTestIndex = 0;
            espconnRegistReconnectCbTestIndex = 0;
            espconnRegistDisconnectCbTestIndex = 0;
            espconnAbortTestIndex = 99;
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
        WHEN("when the connected callback is called successfully but sent CB reg fails")
        {
            espconnRegistSentCbTestIndex = 1;
            espconnRegistRecvCbTestIndex = 0;
            espconnRegistReconnectCbTestIndex = 0;
            espconnRegistDisconnectCbTestIndex = 0;
            espconnAbortTestIndex = 0;
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
        WHEN("when the connected callback is called successfully but received CB reg fails")
        {
            espconnRegistSentCbTestIndex = 0;
            espconnRegistRecvCbTestIndex = 1;
            espconnRegistReconnectCbTestIndex = 0;
            espconnRegistDisconnectCbTestIndex = 0;
            espconnAbortTestIndex = 0;
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
        WHEN("when the connected callback is called successfully but reconnected CB reg fails")
        {
            espconnRegistSentCbTestIndex = 0;
            espconnRegistRecvCbTestIndex = 0;
            espconnRegistReconnectCbTestIndex = 1;
            espconnRegistDisconnectCbTestIndex = 0;
            espconnAbortTestIndex = 0;
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
        WHEN("when the connected callback is called successfully but disconnected CB reg fails")
        {
            espconnRegistSentCbTestIndex = 0;
            espconnRegistRecvCbTestIndex = 0;
            espconnRegistReconnectCbTestIndex = 0;
            espconnRegistDisconnectCbTestIndex = 1;
            espconnAbortTestIndex = 0;
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
