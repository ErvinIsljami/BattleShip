#pragma once
#ifndef LISTOPERATIONS_H
#define LISTOPERATIONS_H

#endif /* LISTOPERATIONS_H */
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

