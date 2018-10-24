#include <stdio.h>
#include <stdlib.h>
#include "NetworkCommands.h"
#include <time.h>
#include <Windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "UserFunctions.h"
#include "ListOperations.h"

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
	srand(time(NULL));
	player *p = (player*)lpParam;
	printf("Player %s started game.\n", p->username);
	LIST * my_list = NULL;
	my_list = get_random_battlefield();
	draw_table(my_list);
	int my_hits = 0;
	int client_hits = 0;
	char *recvBuffer;
	while (true)
	{
		move_command *move_c;
		int len = sizeof(move_command);
		//prima klijentov potez

		int iResult = RecievePacket(p->socket, (char*)&len, 4);
		recvBuffer = (char*)malloc(len + 1);
		iResult = RecievePacket(p->socket, recvBuffer, len);
		move_c = (move_command*)recvBuffer;
		validate_move(move_c->move);
		//provera poteza i validacija... treba implementirati da i server cita random matricu
		server_response response;
		response.code = MOVE_MISS;
		int hit = searchValue(my_list, move_c->move[0] - '0', move_c->move[1] - 'A');
		if (hit == 2)
		{
			response.code = MOVE_HIT;
			client_hits++;
		}
		changeState(&my_list, move_c->move[0] - '0', move_c->move[1] - 'A');

		//odgovara na potez
		len = sizeof(server_response);
		SendPacket(p->socket, (char*)(&len), 4);
		SendPacket(p->socket, (char*)(&response), sizeof(server_response));

		if (client_hits == 17)
			break;

		//pogadja potez
		move_command command;
		command.code = MOVE;
		command.move[0] = rand() % 10 + '0';
		command.move[1] = rand() % 10 + 'A';
		command.move[2] = 0;
		len = sizeof(move_command);
		//salje svoj potez clientu
		SendPacket(p->socket, (char*)(&len), 4);
		SendPacket(p->socket, (char*)(&command), sizeof(move_command));
		if (searchValue(p->ships, command.move[0] - '0', command.move[1] - 'A') == 2)
		{
			my_hits++;
			if (my_hits == 17)
				break;
		}

		free(recvBuffer);
	}
	free(recvBuffer);
	closesocket(p->socket);
	ClearList(&my_list);
	ClearList(&(p->ships));
	return 0;
}

DWORD WINAPI duo_game_thread(LPVOID lpParam)
{
	duo_game *data = (duo_game*)lpParam;
	printf("Igra izmedju %s i %s pocela.\n", data->player_one.username, data->player_two.username);
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