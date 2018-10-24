#include <stdio.h>
#include "ListOperations.h"
#include "Drawing.h"
#pragma warning (disable:4996)
#pragma pack 1
void draw_top_side()
{
	fputs("   " BOX_DDR  BOX_DLR BOX_DLR BOX_DLR, stdout);
	for (int i = 0; i < 9; i++)
		fputs(BOX_DDLR BOX_DLR BOX_DLR BOX_DLR, stdout);
	fputs(BOX_DDL "\n", stdout);
}
void draw_one_row(LIST *head, int row)
{
	char position;
	for (int i = 0; i < 9; i++)
	{
		char position = ' ';
		int s = searchValue(head, row, i);
		if (s == 2)
			position = 254;
		else if (s == 1)
			position = 'X';
		else if (s == -1)
			position = 'O';

		fputs(BOX_DUD, stdout);
		printf(" %c ", position);
	}
	int temp = searchValue(head, row, 9);
	if (temp == 2)
		position = 254;
	else if (temp == -1)
		position = 'O';
	else if (temp == 1)
		position = 'X';
	else
		position = ' ';

	fputs(BOX_DUD, stdout);
	printf(" %c ", position);
	fputs(BOX_DUD"\n", stdout);


	fputs("   " BOX_DUDR BOX_DLR BOX_DLR BOX_DLR, stdout);
	for (int i = 0; i < 9; i++)
		fputs(BOX_DUDLR BOX_DLR BOX_DLR BOX_DLR, stdout);
	fputs(BOX_DUDL "\n", stdout);
}
void draw_bottom_side(LIST *head, int row)
{
	printf("  ");
	for (int i = 0; i < 10; i++)
	{
		char position = ' ';
		int s = searchValue(head, row, i);
		if (s == 2)
			position = 254;
		else if (s == 1)
			position = 'X';
		else if (s == -1)
			position = 'O';

		fputs(BOX_DUD, stdout);
		printf(" %c ", position);
	}
	fputs(BOX_DUD "\n", stdout);

	printf("   ");
	fputs(BOX_DUR BOX_DLR BOX_DLR BOX_DLR, stdout);
	for (int i = 0; i < 9; i++)
		fputs(BOX_DULR BOX_DLR BOX_DLR BOX_DLR, stdout);
	fputs(BOX_DUL "\n", stdout);
}

void draw_table(LIST *head)
{
	char redovi = 'A';

	draw_top_side();
	for (int i = 0; i < 9; i++)
	{
		printf("%d  ", i);
		draw_one_row(head, i);
	}
	printf("9");
	draw_bottom_side(head, 9);
	printf("     ");
	for (int i = 0; i <= 9; i++)
	{
		printf("%C   ", i + redovi);
	}
	printf("\n");
}