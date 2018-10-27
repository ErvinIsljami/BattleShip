#include "NetworkCommands.h"
#pragma warning (disable:4996)
#pragma pack 1

int SendPacket(SOCKET socket, char * message, int messageSize)
{
	int poslao = 0;
	int msgSize = messageSize;
	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(socket, &writefds);
	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;
	unsigned long  mode = 1;
	if (ioctlsocket(socket, FIONBIO, &mode) != 0)
		printf("ioctlsocket failed with error.");
	do
	{
		FD_SET(socket, &writefds);
		int result = select(0, NULL, &writefds, NULL, &timeVal);
		if (result > 0)
		{
			if (FD_ISSET(socket, &writefds))
			{
				int iResult = send(socket, message + poslao, messageSize - poslao, 0);
				if (iResult == SOCKET_ERROR)
				{
					return WSAGetLastError();
				}
				//printf("Poslao %s\n", message + poslao);
				poslao += iResult;
				msgSize -= iResult;
				if (msgSize < 0)
				{
					return -1;
				}
			}
		}
		FD_CLR(socket, &writefds);
	} while (msgSize > 0);

	return 1;
}

int RecievePacket(SOCKET socket, char * recvBuffer, int length)
{
	int primio = 0;
	int len = length;

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(socket, &readfds);

	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	unsigned long  mode = 1;
	if (ioctlsocket(socket, FIONBIO, &mode) != 0)
		printf("ioctlsocket failed with error.");


	do
	{
		FD_SET(socket, &readfds);
		int result = select(0, &readfds, NULL, NULL, &timeVal);
		if (result > 0)
		{
			if (FD_ISSET(socket, &readfds))
			{
				int iResult = recv(socket, recvBuffer + primio, length - primio, 0);
				primio += iResult;
				if (iResult > 0)
				{
					//printf("Primio %s\n", recvBuffer);
				}
				else if (iResult == 0)
				{
					// connection was closed gracefully
					printf("Connection with client closed.\n");
					closesocket(socket);
				}
				else
				{
					// there was an error during recv
					printf("recv failed with error: %d\n", WSAGetLastError());
					return WSAGetLastError();
				}
				len -= iResult;
				if (len < 0)
				{
					printf("Greska primio vise nego sto treba.");
					return -1;
				}
				FD_CLR(socket, &readfds);
			}

		}

	} while (len > 0);

	return 1;
}

bool validate_move(char move[])
{
	if (islower(move[0]))
	{
		move[0] -= 32;
	}
	if (islower(move[1]))
	{
		move[1] -= 32;
	}
	if (move[0] <= 'J' && move[0] >= 'A')
	{
		char temp = move[0];
		move[0] = move[1];
		move[1] = temp;
	}
	if (move[0] > '9' || move[0] < '0')
	{
		return false;
	}
	if (move[1] < 'A' || move[1] > 'J')
	{
		return false;
	}
}

DWORD WINAPI solo_game_thread(LPVOID lpParam)
{
	player *p = (player*)lpParam;
	printf("Player %s started game.\n", p->username);
	LIST * my_list = NULL;
	my_list = get_random_battlefield();
	
	int my_hits = 0;
	int client_hits = 0;
	char *recvBuffer = NULL;
	int tempMove = 0;
	int state = 0;
	unsigned long  mode = 1;
	if (ioctlsocket(p->socket, FIONBIO, &mode) != 0)
		printf("ioctlsocket failed with error.");

	while (true)
	{
		move_command *move_c = NULL;
		
		int len;
		server_response response;
		//prima klijentov potez
		int iResult;
		//implementiran FSM za igranje na serveru
		// state 0 = prima potez od igraca
		// state 1 = salje miss/hit
		// state 2 = salje svoj potez
		if (state == 0)	//rec client move
		{
			do
			{
				fd_set readfds;
				FD_ZERO(&readfds);
				FD_SET(p->socket, &readfds);

				timeval timeVal;
				timeVal.tv_sec = 1;
				timeVal.tv_usec = 0;
				int result = select(0, &readfds, NULL, NULL, &timeVal);
				if (!(result > 0))
					continue;

				if (FD_ISSET(p->socket, &readfds))
				{
					iResult = RecievePacket(p->socket, (char*)&len, 4);
					recvBuffer = (char*)malloc(len + 1);
					iResult = RecievePacket(p->socket, recvBuffer, len);
					move_c = (move_command*)recvBuffer;
					state = 1;
				}
				FD_CLR(p->socket, &readfds);
				FD_ZERO(&readfds);
			} while (state == 0);
		}


		if (state == 1)
		{
			response.code = MOVE_MISS;
			int hit = searchValue(my_list, move_c->move[0] - '0', move_c->move[1] - 'A');
			if (hit == 2)
			{
				response.code = MOVE_HIT;
				client_hits++;
			}
			changeState(&my_list, move_c->move[0] - '0', move_c->move[1] - 'A');
			free(recvBuffer);
			do
			{
				fd_set writefds;
				FD_ZERO(&writefds);
				FD_SET(p->socket, &writefds);
				timeval timeVal;
				timeVal.tv_sec = 1;
				timeVal.tv_usec = 0;
				int result = select(0, NULL, &writefds, NULL, &timeVal);
				if (!(result > 0))
					continue;
				if (FD_ISSET(p->socket, &writefds))
				{
					//odgovara na potez
					len = sizeof(server_response);
					SendPacket(p->socket, (char*)(&len), 4);
					SendPacket(p->socket, (char*)(&response), sizeof(server_response));
					state = 2;
				}
				FD_CLR(p->socket, &writefds);
				FD_ZERO(&writefds);
			} while (state == 1);
		}
		
		if (client_hits == 17)
			break;

		//pogadja potez
		move_command command;
		command.code = MOVE;
		command.move[0] = tempMove / 10 + '0';
		command.move[1] = tempMove % 10 + 'A';
		tempMove++;
		command.move[2] = 0;

		if (state == 2)
		{
			do
			{
				fd_set writefds;
				FD_ZERO(&writefds);
				FD_SET(p->socket, &writefds);

				timeval timeVal;
				timeVal.tv_sec = 1;
				timeVal.tv_usec = 0;
				int result = select(0, NULL, &writefds, NULL, &timeVal);
				if (!(result > 0))
					continue;

				if (FD_ISSET(p->socket, &writefds))
				{
					len = sizeof(move_command);
					SendPacket(p->socket, (char*)(&len), 4);
					SendPacket(p->socket, (char*)(&command), sizeof(move_command));
					state = 0;
				}
			} while (state == 2);

			int ship_state = searchValue(p->ships, command.move[0] - '0', command.move[1] - 'A');
			if (ship_state == 2 || ship_state == 1)
			{
				changeState(&(p->ships), command.move[0] - '0', command.move[1] - 'A');
				my_hits++;

			}
		}

		
		if (my_hits == 17)
			break;
	}
	
	closesocket(p->socket);
	ClearList(&my_list);
	ClearList(&(p->ships));
	return 0;
}

DWORD WINAPI duo_game_thread(LPVOID lpParam)
{
	duo_game *data = (duo_game*)lpParam;
	printf("Igra izmedju %s i %s pocela.\n", data->player_one.username, data->player_two.username);
	draw_table(data->player_one.ships);
	draw_table(data->player_two.ships);
	while (true)
	{
#pragma region recv(p1, move1);
		//prvi klijent igra
		move_command *move_c1;
		int len1 = sizeof(move_command);
		int iResult = RecievePacket(data->player_one.socket, (char*)&len1, 4);
		char *recvBuffer1 = (char*)malloc(len1 + 1);
		iResult = RecievePacket(data->player_one.socket, recvBuffer1, len1);
		move_c1 = (move_command*)recvBuffer1;
		validate_move(move_c1->move);

#pragma endregion

#pragma region send(p1, hit/miss);
		server_response response1;
		response1.code = MOVE_MISS;
		if (searchValue(data->player_two.ships, move_c1->move[0] - '0', move_c1->move[1] - 'A') == 2)
		{
			changeState(&(data->player_two.ships), move_c1->move[0] - '0', move_c1->move[1] - 'A');
			response1.code = MOVE_HIT;
		}
		len1 = sizeof(server_response);
		SendPacket(data->player_one.socket, (char*)(&len1), 4);
		SendPacket(data->player_one.socket, (char*)(&response1), sizeof(server_response));
#pragma endregion

#pragma region recv(p2, move2);
		//drugi klijent igra
		move_command *move_c2;
		int len2 = sizeof(move_command);
		iResult = RecievePacket(data->player_two.socket, (char*)&len2, 4);
		char *recvBuffer2 = (char*)malloc(len2 + 1);
		iResult = RecievePacket(data->player_two.socket, recvBuffer2, len2);
		move_c2 = (move_command*)recvBuffer2;
		validate_move(move_c2->move);
#pragma endregion

#pragma region send(p2, hit/miss);
		server_response response2;
		response2.code = MOVE_MISS;
		if (searchValue(data->player_one.ships, move_c2->move[0] - '0', move_c2->move[1] - 'A') == 2)
		{
			changeState(&(data->player_one.ships), move_c2->move[0] - '0', move_c2->move[1] - 'A');
			response2.code = MOVE_HIT;
		}
		len2 = sizeof(server_response);
		SendPacket(data->player_two.socket, (char*)(&len2), 4);
		SendPacket(data->player_two.socket, (char*)(&response2), sizeof(server_response));
#pragma endregion


#pragma region send(p1, move2);
		move_command command1;
		command1.code = MOVE;
		command1.move[0] = move_c2->move[0];
		command1.move[1] = move_c2->move[1];
		command1.move[2] = 0;
		len1 = sizeof(move_command);
		SendPacket(data->player_one.socket, (char*)(&len1), 4);
		SendPacket(data->player_one.socket, (char*)(&command1), sizeof(move_command));
#pragma endregion

#pragma region send(p2, move1);
		move_command command2;
		command2.code = MOVE;
		command2.move[0] = move_c1->move[0];
		command2.move[1] = move_c1->move[1];
		command2.move[2] = 0;
		len2 = sizeof(move_command);
		SendPacket(data->player_two.socket, (char*)(&len2), 4);
		SendPacket(data->player_two.socket, (char*)(&command2), sizeof(move_command));
#pragma endregion
		free(recvBuffer1);
		free(recvBuffer2);
	}

	ClearList(&(data->player_one.ships));
	ClearList(&(data->player_two.ships));


	return 0;
}

DWORD WINAPI solo_game_thread_test(LPVOID lpParam)
{
	SOCKET socket = *((SOCKET*)lpParam);
	LIST * my_list = get_random_battlefield_test();

	LIST *ships = NULL;
	int my_hits = 0;
	int client_hits = 0;
	char *recvBuffer = NULL;
	int tempMove = 0;
	int state = 0;
	int iResult;
	unsigned long  mode = 1;
	if (ioctlsocket(socket, FIONBIO, &mode) != 0)
		printf("ioctlsocket failed with error.");

	while (true)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(socket, &readfds);

		timeval timeVal;
		timeVal.tv_sec = 1;
		timeVal.tv_usec = 0;
		int result = select(0, &readfds, NULL, NULL, &timeVal);
		if (!(result > 0))
			continue;

		if (FD_ISSET(socket, &readfds))
		{
			int len;
			iResult = RecievePacket(socket, (char*)&len, 4);
			recvBuffer = (char*)malloc(len);
			iResult = RecievePacket(socket, recvBuffer, len);
			if (recvBuffer[0] == NEW_SOLO_GAME)
			{
				start_command *command = (start_command*)(recvBuffer);
				for (int j = 0; j < command->matrix_size; j++)
				{
					PushFront(&ships, command->sparse_matrix[j]); //free in solo_game_thread
				}
				free(recvBuffer);
				printf("%d Started game.\n", socket);
				break;
			}

			
		}
		FD_CLR(socket, &readfds);
		FD_ZERO(&readfds);
	}



	while (true)
	{
		move_command *move_c = NULL;

		int len;
		server_response response;
		//prima klijentov potez
		int iResult;
		//implementiran FSM za igranje na serveru
		// state 0 = prima potez od igraca
		// state 1 = salje miss/hit
		// state 2 = salje svoj potez
		if (state == 0)	//rec client move
		{
			do
			{
				fd_set readfds;
				FD_ZERO(&readfds);
				FD_SET(socket, &readfds);

				timeval timeVal;
				timeVal.tv_sec = 1;
				timeVal.tv_usec = 0;
				int result = select(0, &readfds, NULL, NULL, &timeVal);
				if (!(result > 0))
					continue;

				if (FD_ISSET(socket, &readfds))
				{
					iResult = RecievePacket(socket, (char*)&len, 4);
					recvBuffer = (char*)malloc(len);
					iResult = RecievePacket(socket, recvBuffer, len);
					move_c = (move_command*)recvBuffer;
					state = 1;
				}
				FD_CLR(socket, &readfds);
				FD_ZERO(&readfds);
			} while (state == 0);
		}


		if (state == 1)
		{
			response.code = MOVE_MISS;
			int hit = searchValue(my_list, move_c->move[0] - '0', move_c->move[1] - 'A');
			if (hit == 2)
			{
				response.code = MOVE_HIT;
				client_hits++;
			}
			changeState(&my_list, move_c->move[0] - '0', move_c->move[1] - 'A');
			free(recvBuffer);
			do
			{
				fd_set writefds;
				FD_ZERO(&writefds);
				FD_SET(socket, &writefds);
				timeval timeVal;
				timeVal.tv_sec = 1;
				timeVal.tv_usec = 0;
				int result = select(0, NULL, &writefds, NULL, &timeVal);
				if (!(result > 0))
					continue;
				if (FD_ISSET(socket, &writefds))
				{
					//odgovara na potez
					len = sizeof(server_response);
					SendPacket(socket, (char*)(&len), 4);
					SendPacket(socket, (char*)(&response), sizeof(server_response));
					state = 2;
				}
				FD_CLR(socket, &writefds);
				FD_ZERO(&writefds);
			} while (state == 1);
		}

		if (client_hits == 17)
			break;

		//pogadja potez
		move_command command;
		command.code = MOVE;
		command.move[0] = tempMove / 10 + '0';
		command.move[1] = tempMove % 10 + 'A';
		tempMove++;
		command.move[2] = 0;

		if (state == 2)
		{
			do
			{
				fd_set writefds;
				FD_ZERO(&writefds);
				FD_SET(socket, &writefds);

				timeval timeVal;
				timeVal.tv_sec = 1;
				timeVal.tv_usec = 0;
				int result = select(0, NULL, &writefds, NULL, &timeVal);
				if (!(result > 0))
					continue;

				if (FD_ISSET(socket, &writefds))
				{
					len = sizeof(move_command);
					SendPacket(socket, (char*)(&len), 4);
					SendPacket(socket, (char*)(&command), sizeof(move_command));
					state = 0;
				}
			} while (state == 2);

			int ship_state = searchValue(ships, command.move[0] - '0', command.move[1] - 'A');
			if (ship_state == 2 || ship_state == 1)
			{
				changeState(&(ships), command.move[0] - '0', command.move[1] - 'A');
				my_hits++;

			}
		}


		if (my_hits == 17)
			break;
	}

	closesocket(socket);
	ClearList(&my_list);
	ClearList(&(ships));
	return 0;
}

bool InitializeWindowsSockets()
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

int BindServerSocket(SOCKET *listenSocket, int SERVER_PORT)
{
	// Initialize serverAddress structure used by bind
	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;				// IPv4 address family
	serverAddress.sin_addr.s_addr = INADDR_ANY;		// Use all available addresses
	serverAddress.sin_port = htons(SERVER_PORT);	// Use specific port

													//initialise all client_socket[] to 0 so not checked
	// Create a SOCKET for connecting to server
	*listenSocket = socket(AF_INET,      // IPv4 address family
		SOCK_STREAM,  // Stream socket
		IPPROTO_TCP); // TCP protocol

					  // Check if socket is successfully created
	if (*listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket - bind port number and local address to socket
	int iResult = bind(*listenSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

	// Check if socket is successfully binded to address and port from sockaddr_in structure
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(*listenSocket);
		WSACleanup();
		return -1;
	}
	printf("Server bind ok.\n");
	return 1;
}

int setToListenSocket(SOCKET *listenSocket)
{
	unsigned long  mode = 1;
	if (ioctlsocket(*listenSocket, FIONBIO, &mode) != 0)
		printf("ioctlsocket failed with error.");

	// Set listenSocket in listening mode
	int iResult = listen(*listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(*listenSocket);
		WSACleanup();
		return -1;
	}

	printf("Server socket is set to listening mode. Waiting for new connection requests.\n");

	return 1;
}

bool acceptNewClient(SOCKET *listenSocket, SOCKET *clientSocket)
{
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(struct sockaddr_in);

	*clientSocket = accept(*listenSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

	if (*clientSocket == INVALID_SOCKET)
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		return false;
	}
	else
	{
		unsigned long mode = 1;
		if (ioctlsocket(*clientSocket, FIONBIO, &mode) != 0)
		{
			printf("ioctlsocket failed with error.");
		}
		printf("New client request accepted. Client address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		return true;
	}

	return false;
}