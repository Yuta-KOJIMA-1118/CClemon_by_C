#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <poll.h>

#define NUM_OF_ROOM 100000 // 合計100000部屋までしか遊べないが，そこまで遊ぶ人はいないだろう...

#define LEMON 0
#define FIRE 1
#define BARRIER 2
#define CHANGE 3
#define GUN 4

typedef enum RoomState {
    EMPTY,
    WAITING_FRIEND,
    WAITING_ANYONE,
    PLAYING
} RoomState;

typedef enum SkillType {
    CHARGE,
    ATTACK,
    DEFENCE,
    REFLECT
} SkillType;

typedef struct Skill {
    char name[10];
    int priority; //todo delete
    int energy;
    SkillType type;
} Skill;

typedef struct Player {
    char name[20];
    int next_skill;
    int num_of_lemon;
    int sockfd;
} Player;

typedef struct Room {
    RoomState state;
    Player players[2];
} Room;

typedef struct Battle {
    int room_id;
    int player_num;
} Battle;

extern Skill skills[5];
extern int shm_id;
extern int mutex_shm_id;

// Function Prototypes
void prepare_skills();
void prepare_mutexes(pthread_mutex_t **room_mutexes);
void destroy_mutexes();
pthread_mutex_t *attach_mutexes();
void detach_mutexes(pthread_mutex_t *room_mutexes);
void prepare_shared_memory(Room **rooms);
void init_room(int room_id);
Room *attach_rooms();
void detach_rooms(Room *rooms);
Room *get_room_and_lock(Room *rooms, int room_id);
RoomState get_room_state_no_lock(Room *rooms, int room_id);
void check_room_sockfd(Room *rooms, int room_id);
void unlock_room(int room_id);
int prepare_socket(socklen_t *sin_siz);
void receiver(int new_sockfd);
int room_making(int new_sockfd);
int room_searching(int room_id, int new_sockfd);
void battle_receiver(int room_id, int player_num);
void *pthread_battle_receiver(void *arg);
void battle(int room_id);

#endif // SERVER_H
