
#include <doctest.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_functions.h"

int espconnAcceptTestIndex = 0;
int espconnConnectTestIndex = 0;
int espconnSendTestIndex = 0;
int espconnDisconnectTestIndex = 0;
int espconnAbortTestIndex = 99;

// the mocked calls from UUT to ESPCONN

signed char espconn_accept(struct espconn *espconn)
{
    INFO("This mock is called when the TCP Server configures the ESPCONN library as a listener");

    if (espconnAcceptTestIndex == 0) // a successful return
    {
        INFO("Successful accept");
        REQUIRE_EQ(espconn->proto.tcp->local_port, TEST_PORT_1);
        return 0;
    }
    else if (espconnAcceptTestIndex == 1)
    {
        FAIL("Returns a fail for out of memory");
        REQUIRE_EQ(espconn->proto.tcp->local_port, TEST_PORT_1);
        return ESPCONN_MEM;
    }
    else if (espconnAcceptTestIndex == 2)
    {
        FAIL("Returns a fail for already connected");
        REQUIRE_EQ(espconn->proto.tcp->local_port, TEST_PORT_1);
        return ESPCONN_ISCONN;
    }
    else if (espconnAcceptTestIndex == 3)
    {
        FAIL("Returns a fail for incorrect arguement");
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAcceptTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_connect(struct espconn *espconn)
{
    INFO("This mock is called when the TCP Server connects as a client using the ESPCONN library");

    if (espconnConnectTestIndex == 0)
    {
        INFO("successful connection");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return 0;
    }
    else if (espconnConnectTestIndex == 1)
    {
        FAIL("return a routing problem to the espconn connect");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_RTE;
    }
    else if (espconnConnectTestIndex == 2)
    {
        FAIL("return an out of memory to the espconn connect");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_MEM;
    }
    else if (espconnConnectTestIndex == 3)
    {
        FAIL("return an already connect to the espconn connect");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ISCONN;
    }
    else if (espconnConnectTestIndex == 4)
    {
        FAIL("return a generic arg issue to espconn connect");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnConnectTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}
signed char espconn_send(struct espconn *espconn, unsigned char *psent, unsigned short length)
{
    INFO("This mock is called when the TCP session sends data using ESPCONN library");

    if (espconnSendTestIndex == 0)
    {
        INFO("successful send");
        return 0;
    }
    else if (espconnSendTestIndex == 1)
    {
        FAIL("return a generic arg issue");
        return ESPCONN_ARG;
    }
    else if (espconnSendTestIndex == 2)
    {
        FAIL("return an out-of-memory issue");
        return ESPCONN_MEM;
    }
    else if (espconnSendTestIndex == 3)
    {
        FAIL("return a buffer is full issue");
        return ESPCONN_MAXNUM;
    }
    else
    {
        FAIL("Not yet developed - espconnSendTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_disconnect(struct espconn *espconn)
{
    INFO("This mock is called when the TCP Server disconnects a session using ESPCONN library");

    if (espconnDisconnectTestIndex == 0)
    {
        INFO("successful disconnection");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return 0;
    }
    else if (espconnDisconnectTestIndex == 1)
    {
        FAIL("return a generic arg issue to espconn disconnect");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnDisconnectTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_abort(struct espconn *espconn)
{
    INFO("This mock is called to trigger a session abort");

    if (espconnAbortTestIndex == 0)
    {
        INFO("successful abortion");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return 0;
    }
    else if (espconnAbortTestIndex == 1)
    {
        FAIL("return a generic arg issue to espconn abort");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else if (espconnAbortTestIndex == 99)
    {
        FAIL("Abort called unexpectedly");
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}
