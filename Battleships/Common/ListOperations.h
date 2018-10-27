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

/*openening a file with validating permission for reading or/and writing mode
* @param  file_name  name of file that you are writing or reading
@param  mode  reading or writing mode
@param  err  number that describes your error
*/
FILE *safe_fopen(const char file_name[], const char mode[], int err);

/*Returning the loaded list from txt file
*return random list which is loaded from txt file with random lists
*/
LIST* load_battlefield(char *file_name);

/*Saving the elements in list which user put manualy when he doesn't chooses random generalizing
*
*param head list
*/
void save_battlefield(LIST *head);

/*Generalizing random positions of ships which are loaded from txt file
*return random list which is loaded from txt file with random lists
*/
LIST* get_random_battlefield();

/**
*Function returns the size of list with forwarded list
* @param  head  pointer at the begging of the list
* @return      the size of the list
*/
int GetSize(LIST * head);

/*
*Adding new element at the end of list
* @param  head  pointer of list
* @param  newValue variable of Field type
*/
void PushBack(LIST **head, FIELD newValue);

/*
*Adding new element at the begging of list
* @param  head  pointer of list
* @param  newValue variable of Field type
*/
void PushFront(LIST **head, FIELD newValue);

/*Deleting all elements of the list
* @param  head  pointer of list
*/
void ClearList(LIST **head);

/*Printing all elements of the list
* @param  head  list
*/
void PrintList(LIST * head);

/*Saving list into an array
*@param  list list
*@return the array in which you put the elements of the list
*/
FIELD* list_to_array(LIST* list);

/*Searching the value of the element which is on forwarded position
*@param  head list
*@param  index  position in list which value of the field you want to return
*@return the value of field that is on the forwarded positions in list
*/
FIELD GetNth(struct list_el* head, int index);

/*Searching the value of the element which is on forwarded position
*@param  head list
*@param  rowKey  row position of the field in the table(list)
*@param columnKey column position of the field in the table(list)
*@return the value of field that is on the forwarded positions in list
*/
int searchValue(LIST *head, int rowKey, int columnKey);

/*Deleting the last element of the list and returning its value
*@param  head  pointer of list
*@return the Field which is value of the list and had been deleted through function
*/
FIELD Pop(LIST **head);

/*Deleting the first element of list without returning type
*@param  head  pointer of list
*/
void DeleteFirstElement(LIST **head);

/*Deleting the last element without returning type
*@param  head  list
*/
void RemoveLast(LIST *head);

/*Deleting the element which is on forwarded position
*@param  head  pointer of list
*@param  n  position of element that you want to remove
*@return the Field which is value of the list and had been deleted through function
*/
FIELD RemoveByIndex(LIST ** head, int n);

/*Searching an element on forwarded position in list, if its found you are changing its state from 0 to 1
Otherwise you are creating a new field with state -1 on forwarded position
*@param  head pointer of list
*@param  row row position of an element
*@param  column column position of an element
*/
void shootField(LIST **head, int row, int column);

/*Saving an array into List
*@param  array array
*@return the list in which you put the elements of forwarded array
*/
LIST* arrayToList(FIELD* array, int array_size);

/*Changing state of the field with forwarded position to 1, using when somebody hits or misses oponents field
*@param  head pointer of list
*@param  rowKey row position in list
*@param  columnKey column position in list
*/
void changeState(LIST **head, int rowKey, int columnKey);

/*Generate random battlefield(just for testing purpose)
@return List
*/
LIST* get_random_battlefield_test();

#endif /* LISTOPERATIONS_H */