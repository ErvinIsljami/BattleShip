#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _WIN32_WINNT_WIN10    0x0A00 // Windows 10  
#include <assert.h>

typedef struct field_st
{
	int row;
	int column;
	int state;
}FIELD;

typedef struct list_el
{
	FIELD value;
	struct list_el* next;
}LIST;


int GetSize(LIST * head);
void PushBack(LIST **head, FIELD newValue);
void PushFront(LIST **head, FIELD newValue);
void ClearList(LIST **head);
void PrintList(LIST * head);
//char *SerializeList(LIST * head);
LIST *DeserializeList(char * buffer);
void serializeList(LIST *head, char *buffer);
void* list_to_array(LIST* list);
FIELD GetNth(struct list_el* head, int index);
bool searchValue(LIST **head, int rowKey, int columnKey);

int GetSize(LIST * head)
{
	int i = 0;
	LIST *temp = head;
	while (temp != NULL)
	{
		i++;
		temp = temp->next;
	}
	return i;
}

void PushBack(LIST **head, FIELD newValue)
{
	LIST * new_el = (LIST*)malloc(sizeof(LIST));
	new_el->next = NULL;
	new_el->value = newValue;

	if (*head == NULL)
	{
		*head = new_el;
		return;
	}
	else
	{
		LIST * temp = *head;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = new_el;
	}
}

void PushFront(LIST **head, FIELD newValue)
{
	LIST * new_el = (LIST*)malloc(sizeof(LIST));
	new_el->next = *head;
	new_el->value = newValue;
	*head = new_el;
}

void ClearList(LIST **head)
{
	if (head == NULL)
	{
		return;
	}
	else
	{
		LIST * temp = *head;
		while (*head != NULL)
		{
			*head = temp->next;
			free(temp);
			temp = *head;
		}
	}
}

void PrintList(LIST * head)
{
	printf("List: ");
	LIST * temp = head;
	while (temp != NULL)
	{
		printf("%d, ", temp->value.column);
		printf("%d, ", temp->value.row);
		printf("%d, \n", temp->value.state);
		temp = temp->next;
	}

}

//char *SerializeList(LIST * head)
//{
//	int n = GetSize(head);
//	char * buffer = (char*)malloc(n * sizeof(head->value));
//	LIST * temp = head;
//	int i = 0;
//	memcpy(buffer, &n, sizeof(int));
//
//	while (i < n)
//	{
//		memcpy(buffer + 4 + i * sizeof(temp->value), &(temp->value), sizeof(temp->value));
//		i++;
//	}
//}

LIST *DeserializeList(char * buffer)
{
	int n = *((int*)buffer);
	int i;
	LIST *head = NULL;
	for (i = 0; i < n; i++)
	{
		//PushBack(&head, *((int*)(buffer + 4) + i));
	}

	return head;
}

FIELD Pop(LIST **head)
{
	FIELD retVal;
	retVal.column = -1;
	retVal.row = -1;
	retVal.state = 11;

	LIST * next_node = NULL;
	if (*head == NULL)
	{
		return retVal;
	}
	next_node = (*head)->next;
	retVal = (*head)->value;
	free(*head);
	*head = next_node;

	return retVal;
}

void DeleteFirstElement(LIST **head)
{

	LIST * next_node = NULL;

	if (*head == NULL)
	{
		printf("List is empty\n");
		return;
	}

	next_node = (*head)->next;
	free(*head);
	*head = next_node;

	return;
}

void RemoveLast(LIST *head)
{
	if (head->next == NULL)
	{
		printf("Deleting the only element in list");
		free(head);
	}

	LIST * current = head;
	while (current->next->next != NULL)
	{
		current = current->next;
	}

	free(current->next);
	current->next = NULL;
}

FIELD RemoveByIndex(LIST ** head, int n) {
	int i = 0;

	FIELD retVal;
	retVal.column = -1;
	retVal.row = -1;
	retVal.state = 2;

	LIST * current = *head;
	LIST * temp_node = NULL;

	if (n == 0) {
		return Pop(head);
	}

	for (i = 0; i < n - 1; i++) {
		if (current->next == NULL) {
			return retVal;
		}
		current = current->next;
	}

	temp_node = current->next;

	retVal = temp_node->value;
	current->next = temp_node->next;
	free(temp_node);

	return retVal;

}

bool searchValue(LIST **head, int rowKey, int columnKey)
{
	LIST* temp;
	if ((*head) == NULL)
	{
		printf("The list is empty\n");
	}
	else
	{
		temp = (*head);
		while (temp != NULL)
		{
			if (temp->value.row == rowKey && temp->value.column == columnKey)
			{
				printf("FOUND\n");
				return true;
			}

			temp = temp->next;
		}
		printf("NOT FOUND\n");
		return false;
	}
}

FIELD GetNth(struct list_el* head, int index)
{
	struct list_el* current = head;
	int count = 0;

	while (current != NULL)
	{
		if (count == index)
			return(current->value);
		count++;
		current = current->next;
	}
	//assert(0);
}

void* list_to_array(LIST* list)
{
	int i;
	int array_size = GetSize(list);

	FIELD* array = (FIELD*)malloc(sizeof(FIELD*) * array_size);

	for (i = 0; i < array_size; i++)
	{
		array[i] = GetNth(list, i);
	}

	return array;
}

