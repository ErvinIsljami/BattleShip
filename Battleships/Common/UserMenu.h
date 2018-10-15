#pragma once
#include "ServerCommands.h"
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
			printf("Incorrect choice. Put 0, 1 or 2!\n");
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

	int iResult = send(socket, (char*)(&command), sizeof(register_command), 0);

	// Check result of send function
	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return 1;
	}


	return true;
}



bool log_in(SOCKET socket)
{
	
	char username[10], password[10];

	printf("Username : ");
	scanf("%s", username);
	printf("Password: ");
	scanf("%s", password);

	
	return true;
}