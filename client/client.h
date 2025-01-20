#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // キーボードからEnter無しでの入力用

#include <winsock2.h> // Windows用ソケット リンクには -lws2_32 が必要
#include <pthread.h>

#define LEMON 0
#define FIRE 1
#define BARRIER 2
#define CHANGE 3
#define GUN 4

typedef enum SkillType {
    CHARGE,
    ATTACK,
    DEFENCE,
    REFLECT
} SkillType;

typedef struct Skill {
    char name[10];
    int priority;
    int energy;
    SkillType type;
} Skill;

typedef struct BattleRcvData {
    int e_skill;
    int e_lemon;
    int y_skill;
    int y_lemon;
    int winner;
} BattleRcvData;

extern Skill skills[5];

WSADATA initialize();
void finalize();

void output_home();
void output_with_friend_menu();
void handle_with_friend_menu();
void output_battle_menu(int e_lemon, int y_lemon, int e_skill, int y_skill, int e_prev_skill, int p_prev_skill, int turn, int selected_skill, int mistake);
void handle_battle_menu(int e_lemon, int y_lemon, int e_skill, int y_skill, int e_prev_skill, int y_prev_skill, int *selected_skill);
void output_result(int winner);

int request_room_making(int sockfd);
void waiting_battle(int sockfd, int room_id);
int request_room_searching(int sockfd, int n);
void *pthread_battle_sender(void *arg);
void *pthread_battle_receiver(void *arg);

int prepare_socket();

void start_battle(int sockfd);

void prepare_skills();

#endif // CLIENT_H


