#pragma once
#include "NetworkCommands.h"
#include "Drawing.h"

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

int game_menu(SOCKET socket)
{
	int izbor = 0;
	do
	{
		system("cls");
		printf("*******************MENU*******************\n\n");

		printf("Press number for action: \n");
		printf("0. Log out.\n");
		printf("1. Player vs Computer\n");
		printf("2. Player vs Player\n");

		scanf("%d", &izbor);
		if (izbor == 0)
		{
			return 0;
		}
		else if(izbor != 1 || izbor != 2)
		{
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

	while(true)
	{
		int len;
		int iResult = RecievePacket(socket, (char*)&len, 4);
		if (iResult != 1)
		{
			if (iResult == -1)
			{
				printf("Recieved more bytes than intended.\n");
			}
			else
			{
				printf("Error while getting packets. Error Code: %d\n", iResult);
				closesocket(socket);
				WSACleanup();
				return 1;
			}
		}
		char *recvBuffer = (char*)malloc(len + 1);
		memset(recvBuffer, 0, 1);
		iResult = RecievePacket(socket, recvBuffer, len);
		if (iResult != 1)
		{
			if (iResult == -1)
			{
				printf("Recieved more bytes than intended.\n");
			}
			else
			{
				printf("Error while getting packets. Error Code: %d\n", iResult);
				closesocket(socket);
				WSACleanup();
				return 1;
			}
		}
		if (recvBuffer[0] == REGISTER_OK)
		{
			system("cls");
			printf("Registration succefull.\n");
			printf("Press enter to continue...\n");
			fflush(stdout);
			getchar();
			getchar();
			break;
		}
		else if (recvBuffer[0] == REGISTER_ERROR)
		{
			server_response *response = (server_response*)recvBuffer;
			printf("Registration error: %s\n", response->error);
			break;
		}
	}
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

	while (true)
	{
		int iResult = RecievePacket(socket, (char*)&len, 4);
		if (iResult != 1)
		{
			if (iResult == -1)
			{
				printf("Recieved more bytes than intended.\n");
			}
			else
			{
				printf("Error while getting packets. Error Code: %d\n", iResult);
				closesocket(socket);
				WSACleanup();
				return 1;
			}
		}
		char *recvBuffer = (char*)malloc(len + 1);
		memset(recvBuffer, 0, 1);
		iResult = RecievePacket(socket, recvBuffer, len);
		if (iResult != 1)
		{
			if (iResult == -1)
			{
				printf("Recieved more bytes than intended.\n");
			}
			else
			{
				printf("Error while getting packets. Error Code: %d\n", iResult);
				closesocket(socket);
				WSACleanup();
				return 1;
			}
		}
		if (recvBuffer[0] == LOGIN_OK)
		{
			system("cls");
			printf("Login succefull.\n");
			printf("Press enter to continue...\n");
			fflush(stdout);
			getchar();
			getchar();
			break;
		}
		else if (recvBuffer[0] == LOGIN_ERROR)
		{
			server_response *response = (server_response*)recvBuffer;
			printf("Login error: %s\n", response->error);
			return false;
		}
	}
	
	return true;
}


bool play_game(SOCKET socket)
{
	place_battleships();
	
}

void place_battleships()
{
	draw_table();
	printf("Choose places for your battleships.\n");
	printf("Ships that you have: \n");
	printf("1 ship length 5\n");
	printf("1 ship length 4\n");
	printf("2 ship length 3\n");
	printf("1 ship length 2\n");
	printf("Press enter when you are ready to place your battleships.\n");
	getchar();

	system("cls");
	int number_of_battleships = 5;
	int battleship_array[5] = { 0,1,2,1,1 };
	for (int i = 0; i < number_of_battleships; i++)
	{
		system("cls");
		draw_table();
		for (int j = 0; j < battleship_array[i]; j++)
		{
			do
			{


				printf("Place your %d length battleship\n");
				char begin[3];
				char end[3];
				printf("Enter first block\n");
				scanf("%s\n", begin);
				printf("Enter last block\n");
				scanf("%s\n", end);
				bool is_valid = place_one_battleship(battleship_array[i], begin, end);

			} while (!is_valid);
		}
	}
	
}

bool place_one_battleship(int size, char *begin, char *end)
{
	
}