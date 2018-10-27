#include "UserMenu.h"


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma warning (disable:4996)

#define SERVER_PORT 27016
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10000

// TCP server that use non-blocking sockets
int main()
{ 
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSockets[MAX_CLIENTS];
	player player1;
	player player2;
	int clients_state[MAX_CLIENTS];
	char client_name[10] = "default\0";
	short lastIndex = 0;

	// Variable used to store function return value
	int iResult;

	// Buffer used for storing incoming data
	char dataBuffer[BUFFER_SIZE];
	memset(clientSockets, 0, MAX_CLIENTS * sizeof(SOCKET));

	InitializeWindowsSockets();

	BindServerSocket(&listenSocket, SERVER_PORT);
	
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		clients_state[i] = 0;
	}

	if (setToListenSocket(&listenSocket) == -1)
	{
		//handle
	}

	while (true)
	{
		fd_set readfds;
		timeval timeVal;
		timeVal.tv_sec = 1;
		timeVal.tv_usec = 0;
		FD_ZERO(&readfds);

		if (lastIndex != MAX_CLIENTS)
		{
			FD_SET(listenSocket, &readfds);
		}

		for (int i = 0; i < lastIndex; i++)
		{
			if(clients_state[i] == 0)
				FD_SET(clientSockets[i], &readfds);
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
			printf("bind finished\n");
			if (acceptNewClient(&listenSocket,&clientSockets[lastIndex]))
				lastIndex++;
		}

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
				char *recvBuffer = (char*)malloc(len + 1);
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
				
				switch (recvBuffer[0])
				{
				case REGISTER:
				{
					register_command * command = (register_command*)(recvBuffer);
					bool reg = register_user(command->user);
					if (reg)
					{
						printf("Succes. New user added. \n");
						server_response response;
						response.code = REGISTER_OK;
						int len = sizeof(server_response);
						SendPacket(clientSockets[i], (char*)(&len), 4);
						SendPacket(clientSockets[i], (char*)(command), sizeof(server_response));
					}
					else
					{
						printf("Error while registrating user.\n");
					}
				}


				case LOGIN:
				{
					login_command * command = (login_command*)(recvBuffer);
					bool log = login_user(command->uname, command->pass);
					if (log)
					{
						printf("Success. User %s logged in.\n", command->uname);
						strcpy(client_name, command->uname);
						server_response response;
						response.code = LOGIN_OK;
						int len = sizeof(server_response);
						SendPacket(clientSockets[i], (char*)(&len), 4);
						SendPacket(clientSockets[i], (char*)(&response), sizeof(server_response));
					}
				}

				case NEW_SOLO_GAME:
				{
					start_command * command = (start_command*)(recvBuffer);
					FIELD *desiralized = (FIELD*)command->sparse_matrix;
					player game_data;
					game_data.ships = NULL;
					game_data.socket = clientSockets[i];
					strcpy(game_data.username, client_name);
					for (int j = 0; j < command->matrix_size; j++)
					{
						PushFront(&game_data.ships, command->sparse_matrix[j]); //free in solo_game_thread
					}
					clients_state[i] = 1;
					DWORD id;
					HANDLE game = CreateThread(NULL, 0, &solo_game_thread, &game_data, 0, &id);
				}

				case NEW_DUO_GAME:
				{
					start_command * command = (start_command*)(recvBuffer);
					FIELD *desirialized = (FIELD*)command->sparse_matrix;
					player p;
					p.ships = NULL;
					p.socket = clientSockets[i];
					strcpy(p.username, client_name);
					for (int j = 0; j < command->matrix_size; j++)
					{
						PushFront(&p.ships, desirialized[j]);	//free in duo_game_thread
					}
					bool found = false;
					for (int j = 0; j < MAX_CLIENTS; j++)
					{
						if (i != j && clients_state[j] == 2)
						{
							printf("DUO game begin...\n");
							duo_game game_data;
							game_data.player_one = p;
							game_data.player_two = player1;
							DWORD id;
							HANDLE game = CreateThread(NULL, 0, &duo_game_thread, &game_data, 0, &id);
							found = true;
						}
					}
					if (!found)
						player1 = p;

					clients_state[i] = 2;
				}


				default:
					break;
				}

				free(recvBuffer);
			}
		}
	}
	//Close listen and accepted sockets
	closesocket(listenSocket);

	// Deinitialize WSA library
	WSACleanup();

	return 0;
}