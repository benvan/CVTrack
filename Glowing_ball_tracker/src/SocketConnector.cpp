#include "stdafx.h"
#include "SocketConnector.h"

SocketConnector::SocketConnector(CRITICAL_SECTION criticalSection, Ball* ballPar)
{
	ballCriticalSection = criticalSection;
	ball = ballPar;

	initializeConnector();
}

int SocketConnector::initializeConnector(){
	
	ListenSocket = INVALID_SOCKET, ClientSocket = INVALID_SOCKET;
	result = NULL;
	recvbuflen = DEFAULT_BUFLEN;
	

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT_STR, &hints, &result);
	if ( iResult != 0 ) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, IPPROTO_UDP);//result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);
	printf("Waiting for connection...\n");
	int sentCount=0;
	
	// Receive until the peer shuts down the connection
	do {
			
	Sleep(15);
	EnterCriticalSection(&ballCriticalSection);
	sprintf(sentbuf, "%lf %lf %lf" ,ball->x, ball->y, ball->size);
	LeaveCriticalSection(&ballCriticalSection);
	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	int Port = DEFAULT_PORT;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	iResult = sendto(ListenSocket,sentbuf, strlen(sentbuf),0,(SOCKADDR *) &RecvAddr, sizeof(RecvAddr));
        if (iResult == SOCKET_ERROR){
			printf("Connection closing...\n");
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;

		}
		else  {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 0;
		}

	} while (iResult > 0);
	return 0;
}
int SocketConnector::closeConnector(){
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}


SocketConnector::~SocketConnector(void)
{
}
