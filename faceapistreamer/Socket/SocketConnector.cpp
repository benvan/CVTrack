#include "StdAfx.h"
#include "SocketConnector.h"

SocketConnector::SocketConnector(CRITICAL_SECTION criticalSection, Head* headPar)
{
	headCriticalSection = criticalSection;
	head = headPar;

	printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");



	initializeConnector();
	//closeConnector();


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
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
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

	// Setup the TCP listening socket
	/*
	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}*/

	freeaddrinfo(result);
	printf("Waiting for connection...\n");
/*    iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}*/

	// Accept a client socket
	/*
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
printf("Here12\n");
	// No longer need server socket
	closesocket(ListenSocket);
printf("Here13\n");
*/
	int sentCount=0;
	

	// Receive until the peer shuts down the connection
	do {
			
	//iResult = recv(ListenSocket,recvbuf,recvbuflen,0);
	Sleep(15);
	EnterCriticalSection(&headCriticalSection);
	sprintf(sentbuf, "%lf %lf %lf %lf %lf %lf",head->x, head->y, head->z, 
		head->rx, head->ry, head->rz);
	LeaveCriticalSection(&headCriticalSection);
	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	int Port = 29129;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	iResult = sendto(ListenSocket,sentbuf, strlen(sentbuf),0,(SOCKADDR *) &RecvAddr, sizeof(RecvAddr));
						//(struct sockaddr *)&client_addr, &addr_len);

	   /* iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);*/
		if (iResult > 0) {
		


		/*	static int counter = 0;
			printf("(%d) Bytes sent: %d\n", counter++ , iResult);*/

			
			


		/*	iSendResult = send( ClientSocket, sentbuf, strlen(sentbuf), 0 );
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			Sleep(15);
			printf("%d Bytes sent: %d\n",sentCount++, iSendResult);*/
		}
		else if (iResult == SOCKET_ERROR){
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
