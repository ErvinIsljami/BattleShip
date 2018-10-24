#include <stdio.h>
#include <stdlib.h>
#include "UserMenu.h"
#include "NetworkCommands.h"
#include "Drawing.h"
#pragma warning (disable:4996)
#pragma pack 1

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
		free(recvBuffer);
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
	if (!validate_move(begin))
	{
		return false;
	}
	if (!validate_move(end))
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
	if (mode == 1)
		command.command_id = NEW_SOLO_GAME;
	else
		command.command_id = NEW_DUO_GAME;

	command.mode = mode;
	command.matrix_size = GetSize(*head);
	for (int i = 0; i < command.matrix_size; i++)
	{
		command.sparse_matrix[i] = serialized[i];
	}

	printf("Press enter when ready.\n");
	getchar();

	int len = sizeof(start_command);
	SendPacket(socket, (char*)(&len), 4);
	SendPacket(socket, (char*)(&command), sizeof(start_command));
	free(serialized); //free the aray which was send
					  //ovde krece igra...
	system("mode con: cols=110 lines=65");
	LIST *oponent_list = NULL;
	char move[3];
	system("cls");
	int my_hits = 0;
	int oponent_hits = 0;
	while (true)
	{
		printf("******** Oponents battlefiled ***********\n");
		draw_table(oponent_list);
		printf("********** My battlefiled ***************\n");
		draw_table(*head);
		printf("Guess field: ");
		move_command command;
		command.code = MOVE;
		scanf("%s", command.move);
		FIELD field;
		validate_move(command.move);
		field.row = command.move[0] - '0';
		field.column = command.move[1] - 'A';
		int len = sizeof(start_command);
		//salje svoj potez serveru
		SendPacket(socket, (char*)(&len), 4);
		SendPacket(socket, (char*)(&command), sizeof(start_command));

		//prima odgovor
		int iResult = RecievePacket(socket, (char*)&len, 4);
		char *recvBuffer = (char*)malloc(len + 1);
		memset(recvBuffer, 0, 1);
		iResult = RecievePacket(socket, recvBuffer, len);
		server_response *result = (server_response*)recvBuffer;
		if (result->code == MOVE_HIT)
		{
			field.state = 1;
			my_hits++;
			if (my_hits == 17)
				break;
		}
		else
		{
			field.state = -1;
		}
		PushFront(&oponent_list, field);
		free(recvBuffer);

		iResult = RecievePacket(socket, (char*)&len, 4);
		char *recvBuffer2 = (char*)malloc(len + 1);
		memset(recvBuffer, 0, 1);
		iResult = RecievePacket(socket, recvBuffer2, len);
		if (searchValue(*head, recvBuffer2[4] - '0', recvBuffer2[5] - 'A') == 2)
		{
			oponent_hits++;
			if (oponent_hits == 17)
				break;
		}
		changeState(&(*head), recvBuffer2[4] - '0', recvBuffer2[5] - 'A');
		system("cls");
		free(recvBuffer2);
	}

	ClearList(&oponent_list);
	if (my_hits == 17)
	{
		return true;
	}
	return false;
}