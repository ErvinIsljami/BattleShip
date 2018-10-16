#pragma once
#include "NetworkCommands.h"
char USER_NAME[15];
bool new_user(SOCKET socket);
bool log_in(SOCKET socket);
int user_menu(SOCKET socket);

int user_menu(SOCKET socket) 
{
	int izbor = 0;
	do
	{
		printf("*******************MENU*******************\n\n");

		printf("Press number for action: \n");
		printf("0. Exit.\n");
		printf("1. Register user.\n");
		printf("2. Log in user.\n");

		scanf("%d", &izbor);

		if (izbor == 1) 
		{
			system("CLS");
			new_user(socket);
		}
		else if (izbor == 2) 
		{
			system("CLS");
			log_in(socket);
		}
		else if (izbor == 0) 
		{
			return 0;
		}
		else {
			system("CLS");
			printf("Incorrect choice. \n");
			getchar();
		}
	} while (izbor > 2 || izbor < 0);

	return izbor;
}



bool new_user(SOCKET socket)
{
	register_command command;
	command.command_id = REGISTER;
	printf("**********REGISTER**********\n\n");
	printf("Input name: ");
	scanf("%s", command.user.name);
	printf("Input Lastname: ");
	scanf("%s", command.user.surname);
	printf("Input username: ");
	scanf("%s", command.user.username);
	printf("Input password: ");
	scanf("%s", command.user.password);

	int len = sizeof(register_command);
	SendPacket(socket, (char*)(&len), 4);
	SendPacket(socket, (char*)(&command), sizeof(register_command));

	return true;
}


bool log_in(SOCKET socket)
{
	login_command command;

	command.command_id = LOGIN;
	printf("Username : ");
	scanf("%s", command.uname);
	printf("Password: ");
	scanf("%s", command.pass);
	strcpy(USER_NAME, command.uname);

	int len = sizeof(register_command);
	SendPacket(socket, (char*)(&len), 4);
	SendPacket(socket, (char*)(&command), sizeof(register_command));

	
	return true;
}
