#pragma once
#ifndef USERFUNCTIONS_H
#define USERFUNCTIONS_H

#endif /* USERFUNCTIONS_H */
#include <stdio.h>

#define FILE_NAME "Users.txt"
typedef struct user_st
{
	char name[15];
	char surname[15];
	char username[15];
	char password[15];
}USER;

FILE *safe_fopen(char *file_name, char *mode, int err);
void save_user(USER user);
bool validate_user(USER user);
bool register_user(USER user);
bool login_user(char* uname, char* pass);

