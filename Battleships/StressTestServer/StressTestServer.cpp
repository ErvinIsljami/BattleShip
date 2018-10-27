#include "UserMenu.h"
#define DEFAULT_PORT "27016"
#define MAX_CLIENTS 50000

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma warning (disable:4996)


DWORD WINAPI listenThreadFunction(LPVOID lpParam);
bool InitializeWindowsSockets1();

int  main(void)
{
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET acceptedSocket = INVALID_SOCKET;
	int iResult;

	if (InitializeWindowsSockets1() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}

	addrinfo *resultingAddress = NULL;
	addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4 address
	hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
	hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
	hints.ai_flags = AI_PASSIVE;     // 

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	listenSocket = socket(AF_INET,      // IPv4 address famly
		SOCK_STREAM,  // stream socket
		IPPROTO_TCP); // TCP

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		WSACleanup();
		return 1;
	}

	iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(resultingAddress);

	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server initialized, waiting for clients.\n");

	DWORD id;
	HANDLE listenThread = CreateThread(NULL, 0, &listenThreadFunction, &listenSocket, 0, &id);


	printf("Press enter to stop server.\n");
	getchar();
	getchar();

	// shutdown the connection since we're done
	iResult = shutdown(listenSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(listenSocket);
	closesocket(acceptedSocket);
	WSACleanup();

	return 0;
}

bool InitializeWindowsSockets1()
{
	WSADATA wsaData;
	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

DWORD WINAPI listenThreadFunction(LPVOID lpParam)
{
	SOCKET listenSocket = *((SOCKET*)(lpParam));
	unsigned long mode = 1;
	int iResult = ioctlsocket(listenSocket, FIONBIO, &mode);
	if (iResult != NO_ERROR)
	{
		printf("ioctlsocket za listen failed with error: %ld\n", iResult);
	}

	iResult = listen(listenSocket, SOMAXCONN);

	SOCKET clientSockets[MAX_CLIENTS];

	fd_set readfds;

	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;
	int lastIndex = 0;

	while (true)
	{
		// initialize socket set
		FD_ZERO(&readfds);

		// add server's socket and clients' sockets to set
		if (lastIndex != MAX_CLIENTS)
		{
			FD_SET(listenSocket, &readfds);
		}

		int selectResult = select(0, &readfds, NULL, NULL, &timeVal);

		if (selectResult == SOCKET_ERROR)
		{
			printf("Select failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		else if (selectResult == 0) // timeout expired
		{
			//handle
		}
		else if (FD_ISSET(listenSocket, &readfds))
		{
			// Struct for information about connected client
			sockaddr_in clientAddr;
			int clientAddrSize = sizeof(struct sockaddr_in);

			// New connection request is received. Add new socket in array on first free position.
			clientSockets[lastIndex] = accept(listenSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

			if (clientSockets[lastIndex] == INVALID_SOCKET)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
			}
			else
			{
				if (ioctlsocket(clientSockets[lastIndex], FIONBIO, &mode) != 0)
				{
					printf("ioctlsocket failed with error.");
					continue;
				}
				printf("New client request accepted (%d). Client address: %s : %d\n", lastIndex, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
				DWORD id;
				HANDLE commThread = CreateThread(NULL, 0, &solo_game_thread_test, &clientSockets[lastIndex], 0, &id);
				lastIndex++;
			}
		}
	}

}