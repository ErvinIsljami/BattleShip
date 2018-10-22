#pragma once
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include "UserFunctions.h"
#include "ListOperations.h"
#pragma warning(disable : 4996)
#pragma pack 1
enum command_ids
{
	SHUTDOWN = 0,
	REGISTER,
	LOGIN,
	NEW_SOLO_GAME,
	NEW_DUO_GAME,
	NEW_MOVE,
	REGISTER_OK,
	REGISTER_ERROR,
	LOGIN_OK,
	LOGIN_ERROR,
	GAME_BEGIN,
	GAME_END,
	MOVE,
	MOVE_HIT,
	MOVE_MISS,
	SECOND_PLAYER_MOVE,
	MOVE_TIMEOUT,
	GAME_OVER,
	WINER,
	LOSER
}CLIENT_COMMANDS;

typedef struct register_user_st
{
	char command_id;
	USER user;
}register_command;

typedef struct login_user_st
{
	char command_id;
	char uname[15];
	char pass[15];
}login_command;

typedef struct start_game_st
{
	char command_id;
	char mode;
	FIELD sparse_matrix[17];	//broj ukupno polja x velicina FIELD-a
	int matrix_size;
}start_command;

struct server_response
{
	int code;
	char error[30];
};

typedef struct player_st
{
	SOCKET socket;
	char username[10];
	LIST *ships;
}player;

typedef struct duo_game_st
{
	player player_one;
	player player_two;
}duo_game;

typedef struct player_move_command
{
	int code;
	char move[3];

}move_command;

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

DWORD WINAPI duo_game_thread(LPVOID lpParam)
{
	while (true)
	{
		duo_game *data = (duo_game*)lpParam;
		printf("Igra izmedju %s i %s pocela.\n", data->player_one.username, data->player_two.username);

		Sleep(3000);
	}
}

DWORD WINAPI solo_game_thread(LPVOID lpParam)
{
	srand(time(NULL));
	player *p = (player*)lpParam;
	printf("Player %s started game.\n", p->username);
	while (true)
	{
		move_command move_c;
		int len = sizeof(move_command);
		//prima klijentov potez

		int iResult = RecievePacket(p->socket, (char*)&len, 4);
		char *recvBuffer = (char*)malloc(len + 1);
		memset(recvBuffer, 0, 1);
		iResult = RecievePacket(p->socket, recvBuffer, len);
		
		//provera poteza i validacija... treba implementirati da i server cita random matricu


		//odgovara na potez
		server_response response;
		response.code = MOVE_MISS;
		len = sizeof(server_response);
		SendPacket(p->socket, (char*)(&len), 4);
		SendPacket(p->socket, (char*)(&response), sizeof(server_response));



		//pogadja potez
		move_command command;
		command.code = MOVE;
		command.move[0] = rand() % 10 + '0';
		command.move[1] = rand() % 10 + 'A';
		command.move[2] = 0;
		printf("Server played %s \n", command.move);
		len = sizeof(start_command);
		//salje svoj potez serveru
		SendPacket(p->socket, (char*)(&len), 4);
		SendPacket(p->socket, (char*)(&command), sizeof(move_command));

	}
}