#pragma once
#include "NetworkCommands.h"
#include "Drawing.h"
#include "ListOperations.h"

char USER_NAME[15];
bool new_user(SOCKET socket);
bool log_in(SOCKET socket);
int user_menu(SOCKET socket);
bool place_one_battleship(LIST **head, int length, char begin[], char end[]);

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

		if (izbor == 1 || izbor == 2)
		{
			return izbor;
		}
		else if (izbor == 0)
		{
			return 0;
		}
		else
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

	int len = sizeof(login_command);
	SendPacket(socket, (char*)(&len), 4);
	SendPacket(socket, (char*)(&command), sizeof(login_command));

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

void place_battleships(LIST **head)
{
	draw_table(*head);
	printf("Choose places for your battleships.\n");
	printf("Ships that you have: \n");
	printf("1 ship length 5\n");
	printf("1 ship length 4\n");
	printf("2 ship length 3\n");
	printf("1 ship length 2\n");
	printf("Press enter when you are ready to place your battleships.\n");
	getchar();
	getchar();
	system("cls");

	int number_of_battleships = 5;
	int battleship_array[5] = { 0,1,2,1,1 };
	for (int i = 0; i < number_of_battleships; i++)
	{
		for (int j = 0; j < battleship_array[i]; j++)
		{
			system("cls");
			draw_table(*head);
			bool is_valid = false;
			do
			{
				printf("Place your %d length battleship\n", i + 1);
				char begin[3];
				char end[3];
				printf("Enter first block\n");
				scanf("%s", begin);
				printf("Enter last block\n");
				scanf("%s", end);
				is_valid = place_one_battleship(&(*head), i + 1, begin, end);

				if (is_valid)
				{
					printf("Move valid.\n");
				}
				else
				{
					printf("Move invalid.\n");
				}
				getchar();
				getchar();
			} while (!is_valid);
		}
	}
}

bool place_one_battleship(LIST **head, int length, char begin[], char end[])
{
#pragma region Validate input
	if (islower(begin[0]))
	{
		begin[0] -= 32;
	}
	if (islower(begin[1]))
	{
		begin[1] -= 32;
	}
	if (islower(end[0]))
	{
		end[0] -= 32;
	}
	if (islower(end[1]))
	{
		end[1] -= 32;
	}
	if (begin[0] <= 'J' && begin[0] >= 'A')
	{
		char temp = begin[0];
		begin[0] = begin[1];
		begin[1] = temp;
	}
	if (end[0] <= 'J' && end[0] >= 'A')
	{
		char temp = end[0];
		end[0] = end[1];
		end[1] = temp;
	}
	if (begin[0] > '9' || begin[0] < '0')
	{
		return false;
	}
	if (begin[1] < 'A' || begin[1] > 'J')
	{
		return false;
	}
	if (end[0] > '9' || end[0] < '0')
	{
		return false;
	}
	if (end[1] < 'A' || end[1] > 'J')
	{
		return false;
	}
	if (begin[0] > end[0])
	{
		char temp = begin[0];
		begin[0] = end[0];
		end[0] = temp;
	}
	if (begin[1] > end[1])
	{
		char temp = begin[1];
		begin[1] = end[1];
		end[1] = temp;
	}
#pragma endregion

#pragma region validate move
	if (begin[0] == end[0]) // ista horizontala
	{
		if (end[1] - begin[1] == length - 1) //dobra duzina
		{
			for (int i = 0; i < length; i++)
			{
				FIELD field;
				field.row = begin[0] - '0';
				field.column = begin[1] - 'A' + i;
				field.state = 2;
				PushFront(&(*head), field);
			}
			return true;
		}
	}
	if (begin[1] == end[1]) //ista vertikala
	{
		if (end[0] - begin[0] == length - 1)
		{
			for (int i = 0; i < length; i++)
			{
				FIELD field;
				field.column = begin[1] - 'A';
				field.row = begin[0] - '0' + i;
				field.state = 2;
				PushFront(&(*head), field);
			}
			return true;
		}
	}
#pragma endregion

	return false;
}

bool play_game(SOCKET socket, LIST **head, int mode)
{
	FIELD *serialized = list_to_array(*head);
	start_command command;
	command.command_id = NEW_SOLO_GAME;
	command.mode = mode;
	command.matrix_size = GetSize(*head);
	for (int i = 0; i < command.matrix_size; i++)
	{
		command.sparse_matrix[i] = serialized[i];
	}
	//system("cls");
	printf("Testing serialization:\n");
	for (int i = 0; i < command.matrix_size; i++)
	{
		printf("(%d, %d) -> %d \n", command.sparse_matrix[i].row, command.sparse_matrix[i].column, command.sparse_matrix[i].state);
	}
	getchar();

	getchar();

	int len = sizeof(start_command);
	SendPacket(socket, (char*)(&len), 4);
	SendPacket(socket, (char*)(&command), sizeof(start_command));








	return false;
}