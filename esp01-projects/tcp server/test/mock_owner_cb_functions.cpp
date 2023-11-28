

#include <doctest.h>

#include "tcp_server.h"

#include "mock_owner_cb_functions.h"
#include "mock_espconn_regist_cb.h"

bool connectedCbCalled = false;
bool disconnectedCbCalled = false;
bool reconnectCbCalled = false;
bool sentCbCalled = false;
bool receivedCbCalled = false;

DummyObject dummyObject;

void disconnectedCb(void *ownerObj, TcpSession::TcpSessionPtr session)
{
    disconnectedCbCalled = true;
};

void incomingMessageCb(void *ownerObj, char *pdata, unsigned short len, TcpSession::TcpSessionPtr session)
{
    receivedCbCalled = true;
};

void sentCb(void *ownerObj, TcpSession::TcpSessionPtr session)
{
    sentCbCalled = true;
};

void reconnectCb(void *ownerObj, signed char err, TcpSession::TcpSessionPtr session)
{
    reconnectCbCalled = true;
};

void connectedCb(void *ownerObj, TcpSession::TcpSessionPtr session)
{
    REQUIRE_EQ(ownerObj, (void *)&dummyObject);
    TcpServer &tcpServer = TcpServer::getInstance();
    REQUIRE_EQ(session->isSessionValid(), true);
    REQUIRE_EQ(tcpServer.getSessionCount(), 1);

    bool imrc = session->registerIncomingMessageCb(incomingMessageCb, (void *)&dummyObject);
    espconnRegistRecvCbTestIndex != 0 ? REQUIRE_EQ(imrc, false) : REQUIRE_EQ(imrc, true);
    if (imrc)
    {
        bool msrc = session->registerMessageSentCb(sentCb, (void *)&dummyObject);
        espconnRegistSentCbTestIndex != 0 ? REQUIRE_EQ(msrc, false) : REQUIRE_EQ(msrc, true);
        if (msrc)
        {
            bool sdrc = session->registerSessionDisconnectedCb(disconnectedCb, (void *)&dummyObject);
            espconnRegistDisconnectCbTestIndex != 0 ? REQUIRE_EQ(sdrc, false) : REQUIRE_EQ(sdrc, true);
            if (sdrc)
            {
                bool srrc = session->registerSessionReconnectCb(reconnectCb, (void *)&dummyObject);
                espconnRegistReconnectCbTestIndex != 0 ? REQUIRE_EQ(srrc, false) : REQUIRE_EQ(srrc, true);
            }
        }
    }

    connectedCbCalled = true;
};