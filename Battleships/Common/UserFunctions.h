#pragma once
#ifndef USERFUNCTIONS_H
#define USERFUNCTIONS_H

#include "NetworkCommands.h"

#define FILE_NAME "Users.txt"


void save_user(USER user);
bool validate_user(USER user);
bool register_user(USER user);
bool login_user(char* uname, char* pass);

#endif /* USERFUNCTIONS_H */