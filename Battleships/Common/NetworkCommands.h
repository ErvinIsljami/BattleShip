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

enum command_ids
{
	SHUTDOWN = 0,
	REGISTER,
	LOGIN,
	NEW_SOLO_GAME,
	NEW_DUO_GAME,
	NEW_MOVE
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

enum server_response_st
{
	REGISTER_OK = 0,
	REGISTER_ERROR,
	LOGIN_OK,
	LOGIN_ERROR,
	GAME_BEGIN,
	GAME_END,
	POSITIONS_OK,
	POSITIONS_ERROR,
	MOVE_OK,
	MOVE_ERROR,
	SECOND_PLAYER_MOVE,
	MOVE_TIMEOUT,
	GAME_OVER,
	WINER,
	LOSER
}SERVER_RESPONSE_CODE;

struct server_response
{
	int code;
	char error[30];
};

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