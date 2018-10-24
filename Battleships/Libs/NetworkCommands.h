#pragma once
#ifndef NETWORKCOMMANDS_H
#define NETWORKCOMMANDS_H

#endif /* NETWORKCOMMANDS_H */


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

int SendPacket(SOCKET socket, char * message, int messageSize);
int RecievePacket(SOCKET socket, char * recvBuffer, int length);
bool validate_move(char move[]);
DWORD WINAPI solo_game_thread(LPVOID lpParam);
DWORD WINAPI duo_game_thread(LPVOID lpParam);

