#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <sys/shm.h> // 共有メモリ

#define NUM_OF_ROOM 1000

typedef enum RoomState {
    EMPTY,
    WAITING_FRIEND,
    WAITING_ANYONE,
    PLAYING
}RoomState;

typedef enum SkillType {
    CHARGE,
    ATTACK,
    DEFENCE,
    REFLECT
}SkillType;

typedef struct Skill {
    char name[10];
    int priority;
    int energy;
    SkillType type;
}Skill;

typedef struct Player {
    char name[20];
    Skill next_skill;
    int sockfd;
}Player;

typedef struct Room {
    RoomState state;
    Player players[2];
}Room;

typedef enum SkillName {
    LEMON,
    FIRE,
    BARRIER,
    CHANGE,
    GUN
}SkillName;

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

// グローバル
Skill skills[5];
pthread_mutex_t room_mutexes[NUM_OF_ROOM];

int main() {
    prepare_skills();

    int shm_id;
    Room *rooms;
    prepare_shared_memory(&shm_id, &rooms);

    int sockfd;
    socklen_t sin_siz;
    sockfd = prepare_socket(&sin_siz);

    while(1) {
        int new_sockfd;
        struct sockaddr_in clnt;
        if((new_sockfd = accept(sockfd, (struct sockaddr *)&clnt, &sin_siz)) < 0) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        switch(pid) {
            case -1:
                perror("fork");
                break;

            case 0: // 子プロセス
                printf("connect from %s: %d\n", inet_ntoa(clnt.sin_addr), ntohs(clnt.sin_port));
                receiver(new_sockfd, shm_id);
                //todo close
                //close(new_sockfd);
                exit(0);

            default: // 親プロセス
                //todo close
                //close(new_sockfd);
                break;
        }
    }
    close(sockfd);
    shmdt(rooms);
    shmctl(shm_id, IPC_RMID, NULL);
    destroy_mutexes();
    return 0;
}

void prepare_mutexes() {
    for(int i=0; i<NUM_OF_ROOM; i++) {
        if(pthread_mutex_init(&room_mutexes[i], NULL) != 0) {
            perror("pthread_mutex_init");
            exit(1);
        }
    }
}

void destroy_mutexes() {
    for(int i=0; i<NUM_OF_ROOM; i++) {
        if(pthread_mutex_destroy(&room_mutexes[i]) != 0) {
            perror("pthread_mutex_destroy");
            exit(1);
        }
    }
}

void prepare_skills() {
    strcpy(skills[0].name, "lemon");
    skills[0].priority = 0;
    skills[0].energy = -1;
    skills[0].type = CHARGE;

    strcpy(skills[1].name, "fire");
    skills[1].priority = 1;
    skills[1].energy = 3;
    skills[1].type = ATTACK;

    strcpy(skills[2].name, "barrier");
    skills[2].priority = 2;
    skills[2].energy = 0;
    skills[2].type = DEFENCE;

    strcpy(skills[3].name, "change");
    skills[3].priority = 3;
    skills[3].energy = 1;
    skills[3].type = REFLECT;

    strcpy(skills[4].name, "gun");
    skills[4].priority = 4;
    skills[4].energy = 5;
    skills[4].type = ATTACK;
}

void prepare_shared_memory(int *shm_id, Room **rooms) {
    if((*shm_id = shmget(IPC_PRIVATE, sizeof(Room) * NUM_OF_ROOM, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if((*rooms = (Room *)shmat(*shm_id, 0, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    for(int i=0; i<NUM_OF_ROOM; i++) {
        init_room(&(*rooms)[i]);
    }
}

void init_room(Room *room) {
    room->state = EMPTY;
    for(int i=0; i<2; i++) {
        room->players[i].name[0] = '\0';
        room->players[i].next_skill.name[0] = '\0';
        room->players[i].next_skill = skills[0]; // lemon
        room->players[i].sockfd = -1;
    }
}

Room *attach_rooms(int shm_id) {
    Room *rooms;
    if((rooms = (Room *)shmat(shm_id, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    return rooms;
}

void detach_rooms(Room *rooms) {
    if(shmdt(rooms) == -1) {
        perror("shmdt");
        exit(1);
    }
}

Room *get_room_and_lock(Room *rooms, int room_id) {
    if (room_id < 0 || room_id >= NUM_OF_ROOM) {
//todo 終了処理
        fprintf(stderr, "Invalid room_id: %d\n", room_id);
        exit(1);
    }

    // ロックが取得できるまで待機
    while (1) {
        int lock_status = pthread_mutex_lock(&room_mutexes[room_id]);

        if (lock_status == 0) {
            break;
        }
    }

    // 該当する Room を返す
    return &rooms[room_id];
}


void unlock_room(int room_id) {
    if (room_id < 0 || room_id >= NUM_OF_ROOM) {
//todo 終了処理
        fprintf(stderr, "Invalid room_id: %d\n", room_id);
        exit(1);
    }

    if (pthread_mutex_unlock(&room_mutexes[room_id]) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

int prepare_socket(socklen_t *sin_siz) {
    int sockfd;
    struct sockaddr_in serv;
    char *ip = "127.0.0.1";
    int port = 12345;
    
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    printf("socket() called\n");

    serv.sin_family = PF_INET;
    serv.sin_port = htons(port);
    inet_aton(ip, &serv.sin_addr);
    *sin_siz = sizeof(struct sockaddr_in);
    if(bind(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("bind");
        exit(1);
    }
    printf("bind() called\n");

    if(listen(sockfd, SOMAXCONN) < 0) {
        perror("listen");
        exit(1);
    }
    printf("listen() called\n");

    return sockfd;
}

void receiver(int new_sockfd, int shm_id) {
    char buf[100];
    memset(buf, 0, sizeof(buf));
    int len = recv(new_sockfd, buf, BUFSIZ, 0);
    buf[len] = '\0';
    printf("received: %s\n", buf);

    // buf: [label] [data]
    char *label = strtok(buf, " ");
    char *data = strtok(NULL, " ");

    if(strcmp(label, "room_making") == 0) {
        int room_id = room_making(shm_id, new_sockfd);
        printf("room_id: %d\n", room_id);
    }
    else if(strcmp(label, "room_searching") == 0) {
        int room_id = atoi(data);
        room_searching(shm_id, room_id, new_sockfd);
    }
    else {
        printf("unknown label\n");
    }
}

int room_making(int shm_id, int new_sockfd) {
    Room *rooms = attach_rooms(shm_id);
    int room_id;

    for (int i = 1; i < NUM_OF_ROOM; i++) {
        Room *room = get_room_and_lock(rooms, i);
        if (room->state == EMPTY) {
            printf("room_making %d\n", i);
            room->state = WAITING_FRIEND;
            room->players[0].sockfd = new_sockfd;
            room_id = i;
            printf("room_id: %d\n", room_id);
            char *buf;
            sprintf(buf, "room_id %d", room_id);
            send(room->players[0].sockfd, buf, 8, 0);
            unlock_room(i);
            break;
        }
        unlock_room(i);
    }
    detach_rooms(rooms);
    return room_id;
}

int room_searching(int shm_id, int room_id, int new_sockfd) {
    printf("room_searching %d\n", room_id);
    Room *rooms = attach_rooms(shm_id);
    Room *room = get_room_and_lock(rooms, room_id);
    if(room->state == WAITING_FRIEND) {
        room->state = PLAYING;
        room->players[1].sockfd = new_sockfd;
        send(room->players[0].sockfd, "start", 5, 0);
        send(room->players[1].sockfd, "start", 5, 0);
        unlock_room(room_id);
        return 1;
    }
    else {
        unlock_room(room_id);
        return -1;
    }
}


