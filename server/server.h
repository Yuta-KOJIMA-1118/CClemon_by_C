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

#define NUM_OF_ROOM 1000

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
    int priority;
    int energy;
    SkillType type;
} Skill;

typedef struct Player {
    char name[20];
    Skill next_skill;
    int sockfd;
} Player;

typedef struct Room {
    RoomState state;
    Player players[2];
} Room;

extern Skill skills[5];
extern pthread_mutex_t room_mutexes[NUM_OF_ROOM];

// Function Prototypes
void prepare_skills();
void prepare_mutexes();
void destroy_mutexes();
void prepare_shared_memory(int *shm_id, Room **rooms);
void init_room(Room *room);
Room *attach_rooms(int shm_id);
void detach_rooms(Room *rooms);
Room *get_room_and_lock(Room *rooms, int room_id);
void unlock_room(int room_id);
int prepare_socket(socklen_t *sin_siz);
void receiver(int new_sockfd, int shm_id);
int room_making(int shm_id, int new_sockfd);
int room_searching(int shm_id, int room_id, int new_sockfd);

#endif // SERVER_H
