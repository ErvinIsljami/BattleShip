#pragma once
#include "NetworkCommands.h"

#pragma warning (disable:4996)

#define FILE_NAME "Users.txt"
typedef struct user_st
{
	char name[15];
	char surname[15];
	char username[15];
	char password[15];
}USER;

typedef struct player_st
{
	USER user;
	SOCKET socket;
	int index;
	int mode;	//je rezim igranja, 1 za solo, 2 za duo
	bool is_playing;
}PLAYER;

typedef struct single_st
{
	PLAYER player;
	//ovde cemo posle da dodajemo sta nam treba
}SINGLE_PLAYER;

typedef struct multi_st
{
	PLAYER player1;
	PLAYER player2;
	//kasnije
}MULTI_PLAYER;


FILE *safe_fopen(char *file_name, char *mode, int err)
{
	FILE *file = fopen(file_name, mode);
	if (file == NULL)
	{
		printf("Could not open file %s. File error code: %d\n", file_name, err);
	}

	return file;
}

void save_user(USER user)
{
	FILE *fp = safe_fopen(FILE_NAME, "a", 1);
	fprintf(fp, "%s %s %s %s\n", user.name, user.surname, user.username, user.password);
	fclose(fp);
}

bool validate_user(USER user)
{


	FILE *fp = safe_fopen(FILE_NAME, "r", 1);
	USER temp;
	bool found = false;
	while (fscanf(fp, "%s %s %s %s", temp.name, temp.surname, temp.username, temp.password) != EOF)
	{
		if (strcmp(temp.username, user.username) == 0)
		{
			found = true;
			break;
		}
	}
	if (found)
	{
		return false;
	}

	if (strlen(user.password) < 8)
	{
		return false;
	}

	return true;
}

bool register_user(USER user)
{
	if (!validate_user(user))
	{
		return false;
	}

	save_user(user);

	return true;
}

bool login_user(char* uname, char* pass)
{
	FILE *fp = safe_fopen(FILE_NAME, "r", 1);
	USER temp;
	bool found = false;

	while (fscanf(fp, "%s %s %s %s", temp.name, temp.surname, temp.username, temp.password) != EOF)
	{
		if (strcmp(temp.username, uname) == 0 && strcmp(temp.password, pass) == 0)
		{
			found = true;
			break;
		}
	}

	return found;
}

