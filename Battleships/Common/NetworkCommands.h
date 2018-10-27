#pragma once
#ifndef NETWORKCOMMANDS_H
#define NETWORKCOMMANDS_H

#include "Drawing.h"

#pragma region structures


static enum command_ids
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

typedef struct user_st
{
	char name[15];
	char surname[15];
	char username[15];
	char password[15];
}USER;

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

#pragma endregion


/*Sending packet in smaller pieces if buffer doesn't have enough space
*
@param socket
@param message memory location of my message
@param messageSize size of my message
@return 1 if message was successfully sent and -1 if sended message is bigger than forwarded size
*/
int SendPacket(SOCKET socket, char * message, int messageSize);

/*Receiving packet in smaller pieces if buffer doesn't have enough space
*
@param socket
@param message memory location of my message
@param messageSize size of my message
@return 1 if message was successfully sent and -1 is buffer doesn't have enough space to store message
*/
int RecievePacket(SOCKET socket, char * recvBuffer, int length);

/*Validating the players move, it is case insensitive and can handle wrong order but not wrong characters and longer/shorter message
*
@return true if validation was succesfull, otherwise it returns false
*/
bool validate_move(char move[]);

/*Thread for solo game(server vs client)
*
@return true if validation was succesfull, otherwise it returns false
*/
DWORD WINAPI solo_game_thread(LPVOID lpParam);

/*Thread for duo game(client vs client)
*
@return true if validation was succesfull, otherwise it returns false
*/
DWORD WINAPI duo_game_thread(LPVOID lpParam);

/*Thread for solo game(server vs client)
*
@param socket for communication
@return true if validation was succesfull, otherwise it returns false
*/
DWORD WINAPI solo_game_thread_test(LPVOID lpParam);

/*Initilize windows socket
	@return true if initlized was succesfull, otherwise returns false
*/
bool InitializeWindowsSockets();

/*Seting socket to listen mode
@param listenSocket Pointer to listen socket which will be set in listen mode
*/
int setToListenSocket(SOCKET *listenSocket);
/**/
bool acceptNewClient(SOCKET *listenSocket, SOCKET *clientSocket);
int BindServerSocket(SOCKET *listenSocket, int SERVER_PORT);
#endif // /* NETWORKCOMMANDS_H */