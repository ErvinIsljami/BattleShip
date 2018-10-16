#define WIN32_LEAN_AND_MEAN

#include "NetworkCommands.h"
#include "UserFunctions.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma warning (disable:4996)

#define SERVER_PORT 27016
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10

// TCP server that use non-blocking sockets
int main()
{

#pragma region Connection



	// Socket used for listening for new clients 
	SOCKET listenSocket = INVALID_SOCKET;

	// Sockets used for communication with client
	SOCKET clientSockets[MAX_CLIENTS];
	short lastIndex = 0;

	// Variable used to store function return value
	int iResult;

	// Buffer used for storing incoming data
	char dataBuffer[BUFFER_SIZE];

	// WSADATA data structure that is to receive details of the Windows Sockets implementation
	WSADATA wsaData;

	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	// Initialize serverAddress structure used by bind
	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;				// IPv4 address family
	serverAddress.sin_addr.s_addr = INADDR_ANY;		// Use all available addresses
	serverAddress.sin_port = htons(SERVER_PORT);	// Use specific port

													//initialise all client_socket[] to 0 so not checked
	memset(clientSockets, 0, MAX_CLIENTS * sizeof(SOCKET));

	// Create a SOCKET for connecting to server
	listenSocket = socket(AF_INET,      // IPv4 address family
		SOCK_STREAM,  // Stream socket
		IPPROTO_TCP); // TCP protocol

					  // Check if socket is successfully created
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket - bind port number and local address to socket
	iResult = bind(listenSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

	// Check if socket is successfully binded to address and port from sockaddr_in structure
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// All connections are by default accepted by protocol stek if socket is in listening mode.
	// With SO_CONDITIONAL_ACCEPT parameter set to true, connections will not be accepted by default
	//bool bOptVal = true;
	//int bOptLen = sizeof (bool);
	//iResult = setsockopt(listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (char *) &bOptVal, bOptLen);
	//   if (iResult == SOCKET_ERROR) {
	//       printf("setsockopt for SO_CONDITIONAL_ACCEPT failed with error: %u\n", WSAGetLastError());
	//}

	unsigned long  mode = 1;
	if (ioctlsocket(listenSocket, FIONBIO, &mode) != 0)
		printf("ioctlsocket failed with error.");

	// Set listenSocket in listening mode
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server socket is set to listening mode. Waiting for new connection requests.\n");

	// set of socket descriptors
	fd_set readfds;

	// timeout for select function
	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;
#pragma endregion

#pragma region Accept



	while (true)
	{
		// initialize socket set
		FD_ZERO(&readfds);

		// add server's socket and clients' sockets to set
		if (lastIndex != MAX_CLIENTS)
		{
			FD_SET(listenSocket, &readfds);
		}

		for (int i = 0; i < lastIndex; i++)
		{
			FD_SET(clientSockets[i], &readfds);
		}

		// wait for events on set
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
				lastIndex++;
				printf("New client request accepted (%d). Client address: %s : %d\n", lastIndex, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

			}
		}
#pragma endregion

#pragma region Comunication
		// Check if new message is received from connected clients
		for (int i = 0; i < lastIndex; i++)
		{
			// Check if new message is received from client on position "i"
			if (FD_ISSET(clientSockets[i], &readfds))
			{
				int len;
				iResult = RecievePacket(clientSockets[i], (char*)&len, 4);
				if (iResult != 1)
				{
					if (iResult == -1)
					{
						printf("Recieved more bytes than intended.\n");
					}
					else
					{
						printf("Error while getting packets. Error Code: %d\n", iResult);
						closesocket(clientSockets[i]);
						WSACleanup();
						return 1;
					}
				}
				printf("Duzina: %d \n", len);
				char *recvBuffer = (char*)malloc(len + 1);
				memset(recvBuffer, 0, 1);
				iResult = RecievePacket(clientSockets[i], recvBuffer, len);
				if (iResult != 1)
				{
					if (iResult == -1)
					{
						printf("Recieved more bytes than intended.\n");
					}
					else
					{
						printf("Error while getting packets. Error Code: %d\n", iResult);
						closesocket(clientSockets[i]);
						WSACleanup();
						return 1;
					}
				}
				//handle user message
				if (recvBuffer[0] == REGISTER) //
				{
					register_command *command = (register_command*)(recvBuffer);
					bool reg = register_user(command->user);
					if (reg)
					{
						printf("Succes. New user added. \n");
						server_response response;
						response.code = REGISTER_OK;
						int len = sizeof(server_response);
						SendPacket(clientSockets[i], (char*)(&len), 4);
						SendPacket(clientSockets[i], (char*)(&command), sizeof(server_response));
					}
					else
					{
						printf("Error while registrating user.\n");
					}
					
				}
				else if (recvBuffer[0] == LOGIN)
				{
					login_command *command = (login_command*)(recvBuffer);
					bool log = login_user(command->uname, command->pass);
					if (log)
					{
						printf("Success. User %s logged in.\n", command->uname);
						server_response response;
						response.code = LOGIN_OK;
						int len = sizeof(server_response);
						SendPacket(clientSockets[i], (char*)(&len), 4);
						SendPacket(clientSockets[i], (char*)(&command), sizeof(server_response));
					}
				}
				free(recvBuffer);
				// here is where server shutdown loguc could be placed
			}
		}
	}


#pragma endregion
	//Close listen and accepted sockets
	closesocket(listenSocket);

	// Deinitialize WSA library
	WSACleanup();

	return 0;
}