#pragma once
#ifndef USERMENU_H
#define USERMENU_H

#include "UserFunctions.h"

bool new_user(SOCKET socket);
bool log_in(SOCKET socket);
int user_menu(SOCKET socket);
bool place_one_battleship(LIST **head, int length, char begin[], char end[]);
int game_menu(SOCKET socket);
void place_battleships(LIST **head);
bool play_game(SOCKET socket, LIST **head, int mode);
bool play_game_automatic(SOCKET socket, LIST **head, int mode);

#endif /* USERMENU_H */