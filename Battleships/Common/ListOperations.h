#pragma once
#ifndef LISTOPERATIONS_H
#define LISTOPERATIONS_H
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>


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

FILE *safe_fopen(char *file_name, char *mode, int err);
LIST* load_battlefield(char *file_name);
void save_battlefield(LIST *head);
LIST* get_random_battlefield();
int GetSize(LIST * head);
void PushBack(LIST **head, FIELD newValue);
void PushFront(LIST **head, FIELD newValue);
void ClearList(LIST **head);
void PrintList(LIST * head);
LIST *DeserializeList(char * buffer);
FIELD* list_to_array(LIST* list);
FIELD GetNth(struct list_el* head, int index);
int searchValue(LIST *head, int rowKey, int columnKey);
FIELD Pop(LIST **head);
void DeleteFirstElement(LIST **head);
void RemoveLast(LIST *head);
FIELD RemoveByIndex(LIST ** head, int n);
void shootField(LIST **head, int row, int column);
LIST* arrayToList(FIELD* array, int array_size);
void changeState(LIST **head, int rowKey, int columnKey);

#endif /* LISTOPERATIONS_H */