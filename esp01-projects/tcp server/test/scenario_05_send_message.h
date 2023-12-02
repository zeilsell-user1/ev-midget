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

SCENARIO("Messages can be sent")
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.cleanup();

    GIVEN("that a session has been established")
    {
        espconn mockedEspconn;
        unsigned short port = TEST_PORT_1;

        esp_tcp tcp;
        mockedEspconn.type = ESPCONN_TCP;
        mockedEspconn.state = ESPCONN_CONNECT;
        tcp.remote_ip[0] = IP_1;
        tcp.remote_ip[1] = IP_2;
        tcp.remote_ip[2] = IP_3;
        tcp.remote_ip[3] = IP_4;
        tcp.remote_port = TEST_PORT_1;
        mockedEspconn.proto.tcp = &tcp;

        espconnRegistConnectCbTestIndex = 0;
        REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);
        setupTest();
        connectCb_(&mockedEspconn);
        REQUIRE_EQ(connectedCbCalled, true);
        REQUIRE_EQ(testSession->isSessionValid(), true);

        WHEN("when a send is passed to the ESPCONN firmware")
        {
            unsigned char testMessage[] = "This is a test message";
            testSession->sendMessage(testMessage, strlen((const char *)testMessage));
        }
        WHEN("clean up test")
        {
            tcpServer.cleanup();                 // ensure the server is clean from the start
            THEN("tcpServer should be empty again")
            {
                REQUIRE_EQ(tcpServer.getSessionCount(), 0);
            }
        }
        
    }
}
