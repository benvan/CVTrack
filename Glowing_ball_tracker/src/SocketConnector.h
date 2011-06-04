#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT_STR "29130"
#define DEFAULT_PORT 29130

#include "Ball.h"


class SocketConnector
{
private:
    CRITICAL_SECTION ballCriticalSection;
    WSADATA wsaData;
    SOCKET ListenSocket ,
        ClientSocket ;
    struct addrinfo *result,
        hints;
    char recvbuf[DEFAULT_BUFLEN];
    char sentbuf[DEFAULT_BUFLEN];
    int iResult, iSendResult;
    int recvbuflen;
    Ball* ball;




public:
    SocketConnector::SocketConnector(CRITICAL_SECTION criticalSection, Ball* ballPar);
    int initializeConnector();



    int closeConnector();
    ~SocketConnector(void);
};
