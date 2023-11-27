
#ifndef __MOCK_ESPCONN_FUNCTIONS_H__
#define __MOCK_ESPCONN_FUNCTIONS_H__

extern int espconnAcceptTestIndex;
extern int espconnConnectTestIndex;
extern int espconnSendTestIndex;
extern int espconnDisconnectTestIndex;
extern int espconnAbortTestIndex;

extern bool espconnAcceptCalled;
extern bool espconnConnectCalled;
extern bool espconnSendCalled;
extern bool espconnDisconnectCalled;
extern bool espconnAbortTestCalled;

#endif // __MOCK_ESPCONN_FUNCTIONS_H__
