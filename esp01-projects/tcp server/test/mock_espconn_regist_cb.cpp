
#include <doctest.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_regist_cb.h"

int espconnRegistConnectCbTestIndex = 0;
int espconnRegistDisconnectCbTestIndex = 0;
int espconnRegistRecvCbTestIndex = 0;
int espconnRegistSentCbTestIndex = 0;
int espconnRegistReconnectCbTestIndex = 0;

espconn_connect_callback connectCb_;
espconn_sent_callback sentCb_;
espconn_recv_callback recvCb_;
espconn_connect_callback disconnectCb_;
espconn_reconnect_callback reconnectCb_;

// the callback to the UUT

signed char espconn_regist_connectcb(struct espconn *espconn, espconn_connect_callback connCb)
{
    INFO("This mock is called when the TCP Server registers a connect CB with the ESPCONN library");

    if (espconnRegistConnectCbTestIndex == 0)
    {
        INFO("Successful registeration of connect CB");
        REQUIRE_EQ(espconn->proto.tcp->local_port, TEST_PORT_1);
        connectCb_ = connCb;
    }
    else if (espconnRegistConnectCbTestIndex == 1)
    {
        FAIL("return a generic arg issue to espconn abort");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_regist_sentcb(struct espconn *espconn, espconn_sent_callback sentCb)
{
    INFO("This mock is called when the TCP Server registers a sent CB with the ESPCONN library");

    if (espconnRegistSentCbTestIndex == 0)
    {
        INFO("Successful registeration of sent CB");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        sentCb_ = sentCb;
    }
    else if (espconnRegistSentCbTestIndex == 1)
    {
        FAIL("return a generic arg issue to espconn abort");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_regist_recvcb(struct espconn *espconn, espconn_recv_callback recvCb)
{
    INFO("This mock is called when the TCP Server registers a receive CB with the ESPCONN library");

    if (espconnRegistRecvCbTestIndex == 0)
    {
        INFO("Successful registeration of receive CB");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        recvCb_ = recvCb;
    }
    else if (espconnRegistRecvCbTestIndex == 1)
    {
        FAIL("return a generic arg issue to espconn abort");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_regist_reconcb(struct espconn *espconn, espconn_reconnect_callback reconCb)
{
    INFO("This mock is called when the TCP Server registers a reconnect CB with the ESPCONN library");

    if (espconnRegistReconnectCbTestIndex == 0)
    {
        INFO("Successful registeration of reconnect CB");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        reconnectCb_ = reconCb;
    }
    else if (espconnRegistReconnectCbTestIndex == 1)
    {
        FAIL("return a generic arg issue to espconn abort");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_regist_disconcb(struct espconn *espconn, espconn_connect_callback disconCb)
{
    INFO("This mock is called when the TCP Server registers s disconnect CB with the ESPCONN library");

    if (espconnRegistDisconnectCbTestIndex == 0)
    {
        INFO("Successful registeration of disconnect CB");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        disconnectCb_ = disconCb;
    }
    else if (espconnRegistDisconnectCbTestIndex == 1)
    {
        FAIL("return a generic arg issue to espconn register disconnect");
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[0], IP_1);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[1], IP_2);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[2], IP_3);
        REQUIRE_EQ(espconn->proto.tcp->remote_ip[3], IP_4);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}