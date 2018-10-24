#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "UserFunctions.h"
#include "ListOperations.h"
#pragma warning (disable:4996)
#pragma pack 1
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

int searchValue(LIST *head, int rowKey, int columnKey)
{
	LIST* temp;
	if ((head) == NULL)
	{
		return 0;
	}
	else
	{
		temp = (head);
		while (temp != NULL)
		{
			if (temp->value.row == rowKey && temp->value.column == columnKey)
			{
				return temp->value.state;
			}

			temp = temp->next;
		}
	}

	return 0;
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

FIELD* list_to_array(LIST* list)
{
	int i;
	int array_size = GetSize(list);

	FIELD* array = (FIELD*)malloc(sizeof(FIELD) * array_size);

	for (i = 0; i < array_size; i++)
	{
		array[i] = GetNth(list, i);
	}

	return array;
}

void shootField(LIST **head, int row, int column) //pretazuje listu po row&column, ako nadje element na toj poziciji, promeni state iz 0 u 1, a ako ga ne nadje, dodaje novo polje sa state = -1
{
	LIST* temp;
	int count = 0;

	FIELD listField;
	bool nadjen = false;

	if ((*head) == NULL)
	{
		printf("The list is empty\n");
	}
	else
	{
		temp = (*head);
		while (temp != NULL)
		{
			if (temp->value.row == row && temp->value.column == column)
			{
				temp->value.state = 1;
				nadjen = true;
			}
			temp = temp->next;

		}

		if (nadjen == false)
		{
			FIELD novi;
			novi.row = row;
			novi.column = column;
			novi.state = -1;

			PushBack(head, novi);
		}
	}
}

LIST* arrayToList(FIELD* array, int array_size)
{
	LIST* lista = NULL;

	for (int i = 0; i<array_size; i++)
	{
		PushFront(&lista, array[i]);
	}

	return lista;
}

void changeState(LIST **head, int rowKey, int columnKey)
{
	LIST* temp;
	if ((head) == NULL)
	{
		return;
	}
	else
	{
		temp = *(head);
		while (temp != NULL)
		{
			if (temp->value.row == rowKey && temp->value.column == columnKey)
			{
				temp->value.state = 1;
				return;
			}

			temp = temp->next;
		}
	}

	FIELD f;
	f.row = rowKey;
	f.column = columnKey;
	f.state = -1;

	PushFront(&(*head), f);
	return;
}

LIST* get_random_battlefield()
{
	//imaces fajl koji se zove number_of_battlefield.txt, u njemu cuvas ukupan broj matrica, ucitas n
	//uzmes random broj(r) izmedju 1 i n, i koristis sprintf(file_name,"battlefield%d.txt",r);
	//ucitas tu matricu preko load_battlefield funkcije, prosledis joj ime fajla
	//vratis tu listu sto ti je vratila funkcija

	int n = 0;
	srand(time(NULL));
	char ime[20];

	FILE *fp = safe_fopen("number_of_battlefield.txt", "r", 404);
	if (fp == NULL)
	{
		return NULL;
	}
	fscanf(fp, "%d", &n);

	int r = rand() % n + 1;

	sprintf(ime, "battlefield%d.txt", r);

	return load_battlefield(ime);
}

LIST* load_battlefield(char *file_name)
{
	//fajl izgleda ovako: row column value
	//imaces fajlove koji se zovu, battlefield1.txt, battlefield2.txt, battlefield3.txt, battlefield4.tx
	FILE *fp = safe_fopen(file_name, "r", 404);
	if (fp == NULL)
	{
		return NULL;
	}

	LIST* lista = NULL;
	FIELD temp;
	temp.column = -1;
	temp.row = -1;
	temp.state = 10;

	while (fscanf(fp, "%d %d %d", &temp.row, &temp.column, &temp.state) != EOF)
	{
		PushFront(&lista, temp);
	}

	return lista;
}

void save_battlefield(LIST *head)
{
	//ucita broj n iz fajla number_of_battlefield.txt, ako ne postoji taj fajl, napravis ga i upises 0
	//napravis fajl koji se zove sprintf(file_name, "battlefield%d.txt",n+1)
	//upises u taj fajl matricu(moras da ispostujes redosled row column value)
	//povecas n za jedan jer si upisao jednu matricu
	//upises n

	int n = 0;
	char ime[20];

	FILE *fp2;
	FILE *fp = safe_fopen("number_of_battlefield.txt", "r", 404);
	if (fp == NULL)
	{
		fp2 = safe_fopen("number_of_battlefield.txt", "w", 1);
		fprintf(fp2, "0");
		fclose(fp2);
		fp = safe_fopen("number_of_battlefield.txt", "r", 404);
	}

	fscanf(fp, "%d", &n);
	fclose(fp);

	sprintf(ime, "battlefield%d.txt", n + 1);
	FILE *fp3 = safe_fopen(ime, "w", 1);

	LIST* current = head;

	while (current != NULL)
	{
		fprintf(fp3, "%d %d %d\n", current->value.row, current->value.column, current->value.state);

		current = current->next;
	}

	fclose(fp3);

	fp2 = safe_fopen("number_of_battlefield.txt", "w", 1);
	n++;
	fprintf(fp2, "%d", n);
	fclose(fp2);
}