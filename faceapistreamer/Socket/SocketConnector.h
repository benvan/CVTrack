#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "29129"

#include "Head.h"


class SocketConnector
{
private:
	CRITICAL_SECTION headCriticalSection;
	WSADATA wsaData;
    SOCKET ListenSocket ,
           ClientSocket ;
    struct addrinfo *result,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
	char sentbuf[DEFAULT_BUFLEN];
    int iResult, iSendResult;
    int recvbuflen;
	Head* head;


	
	
public:
	SocketConnector::SocketConnector(CRITICAL_SECTION criticalSection, Head* headPar);
	int initializeConnector();
	
	
	
	int closeConnector();
	~SocketConnector(void);
};
