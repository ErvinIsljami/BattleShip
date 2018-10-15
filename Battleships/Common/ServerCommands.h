#pragma once
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include "UserFunctions.h"

#pragma warning(disable : 4996)

enum command_ids
{
	SHUTDOWN = 0,
	REGISTER,
	LOGIN,
	NEW_SOLO_GAME,
	NEW_DUO_GAME,
	NEW_MOVE
};

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