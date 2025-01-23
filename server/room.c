#include "server.h"

void prepare_shared_memory(Room **rooms) {
    if((shm_id = shmget(IPC_PRIVATE, sizeof(Room) * NUM_OF_ROOM, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if((*rooms = (Room *)shmat(shm_id, 0, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    for(int i=0; i<NUM_OF_ROOM; i++) {
        init_room(i);
    }
}

void init_room(int room_id) {
    printf("init_room %d\n", room_id);
    Room *rooms = attach_rooms();
    Room *room = get_room_and_lock(rooms, room_id);
    room->state = EMPTY;
    for(int i=0; i<2; i++) {
        room->players[i].name[0] = '\0';
        room->players[i].next_skill = 0; // lemon
        room->players[i].num_of_lemon = 0;
        close(room->players[i].sockfd);
        room->players[i].sockfd = -2; // -2: no player, -1: player left
    }
    unlock_room(room_id);
}

Room *attach_rooms() {
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

void prepare_mutexes(pthread_mutex_t **room_mutexes) {
    if((mutex_shm_id = shmget(IPC_PRIVATE, sizeof(pthread_mutex_t) * NUM_OF_ROOM, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if((*room_mutexes = (pthread_mutex_t *)shmat(mutex_shm_id, 0, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        perror("pthread_mutexattr_init");
        exit(1);
    }

    // PTHREAD_PROCESS_SHARED を設定 (プロセス間で mutex を共有するばあいは必須)
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        perror("pthread_mutexattr_setpshared");
        exit(1);
    }

    for (int i = 0; i < NUM_OF_ROOM; i++) {
        if (pthread_mutex_init(&(*room_mutexes)[i], &attr) != 0) {
            perror("pthread_mutex_init");
            exit(1);
        }
    }

    // 属性の破棄
    pthread_mutexattr_destroy(&attr);
}

pthread_mutex_t *attach_mutexes() {
    pthread_mutex_t *room_mutexes;
    if((room_mutexes = (pthread_mutex_t *)shmat(mutex_shm_id, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    return room_mutexes;
}

void detach_mutexes(pthread_mutex_t *room_mutexes) {
    if(shmdt(room_mutexes) == -1) {
        perror("shmdt");
        exit(1);
    }
}

void destroy_mutexes() {
    pthread_mutex_t *room_mutexes = attach_mutexes();
    for(int i=0; i<NUM_OF_ROOM; i++) {
        if(pthread_mutex_destroy(&room_mutexes[i]) != 0) {
            perror("pthread_mutex_destroy");
            exit(1);
        }
    }
    detach_mutexes(room_mutexes);
}

Room *get_room_and_lock(Room *rooms, int room_id) {
    printf("get_room_and_lock %d\n", room_id);
    if (room_id < 0 || room_id >= NUM_OF_ROOM) {
        fprintf(stderr, "Invalid room_id: %d\n", room_id);
        exit(1);
    }

    // ロックが取得できるまで待機
    pthread_mutex_t *room_mutexes = attach_mutexes();
    while (1) {
        int lock_status = pthread_mutex_lock(&room_mutexes[room_id]);

        if (lock_status == 0) {
            printf("lock %d\n", room_id);
            break;
        }
    }
    detach_mutexes(room_mutexes);

    // 該当する Room を返す
    printf("get_room_and_lock end %d\n", room_id);
    return &rooms[room_id];
}


void unlock_room(int room_id) {
    if (room_id < 0 || room_id >= NUM_OF_ROOM) {
        fprintf(stderr, "Invalid room_id: %d\n", room_id);
        exit(1);
    }

    pthread_mutex_t *room_mutexes = attach_mutexes();
    if (pthread_mutex_unlock(&room_mutexes[room_id]) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
    printf("unlock %d\n", room_id);
    detach_mutexes(room_mutexes);
}

RoomState get_room_state_no_lock(Room *rooms, int room_id) {
    if (room_id < 0 || room_id >= NUM_OF_ROOM) {
        fprintf(stderr, "Invalid room_id: %d\n", room_id);
        exit(1);
    }

    Room *room = &rooms[room_id];
    return room->state;
}

void check_room_sockfd(Room *rooms, int room_id) {
    printf("check_room_sockfd %d\n", room_id);
    int flag = 0;
    Room *room = get_room_and_lock(rooms, room_id);
    int sockfd[2];
    sockfd[0] = room->players[0].sockfd;
    sockfd[1] = room->players[1].sockfd;
    unlock_room(room_id);
    for(int i=0; i<2; i++) {
        // 実際に通信をしてsockfdが有効かを確認
        if(sockfd[i] != -2) {
            struct pollfd pfd;
            pfd.fd = sockfd[i];
            pfd.events = POLLIN | POLLPRI;
            
            int ret = poll(&pfd, 1, 0);
            if(ret == -1 || pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                flag = 1;
            }

            if(pfd.revents & POLLIN) {
                char buf[30];
                int len = recv(sockfd[i], buf, 30, MSG_PEEK);
                if(len == 0 || len == -1) {
                    flag = 1;
                }
            }
        }
    }

    if(flag == 1) {
        init_room(room_id);
    }
    printf("check_room_sockfd end\n");

}

