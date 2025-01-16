#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // キーボードからEnter無しでの入力用

#include <winsock2.h> // Windows用ソケット リンクには -lws2_32 が必要

WSADATA initialize();
void finalize();

void output_home();
void output_with_friend_menu();
void handle_with_friend_menu();

int request_room_making(int sockfd);
void waiting_battle(int sockfd, int room_id);
int request_room_searching(int sockfd, int n);

int prepare_socket();

void start_battle(int sockfd);

#endif // CLIENT_H


