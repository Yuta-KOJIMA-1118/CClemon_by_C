#include "server.h"

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
        init_room(i, *shm_id);
    }
}

void init_room(int room_id, int shm_id) {
    printf("init_room %d\n", room_id);
    Room *rooms = attach_rooms(shm_id);
    Room *room = get_room_and_lock(rooms, room_id);
    room->state = EMPTY;
    for(int i=0; i<2; i++) {
        room->players[i].name[0] = '\0';
        room->players[i].next_skill = 0; // lemon
        close(room->players[i].sockfd);
        room->players[i].sockfd = -2; // -2: no player, -1: player left
    }
    unlock_room(room_id);
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

Room *get_room_and_lock(Room *rooms, int room_id) {
    if (room_id < 0 || room_id >= NUM_OF_ROOM) {
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
        fprintf(stderr, "Invalid room_id: %d\n", room_id);
        exit(1);
    }

    if (pthread_mutex_unlock(&room_mutexes[room_id]) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

RoomState get_room_state_no_lock(Room *rooms, int room_id) {
    if (room_id < 0 || room_id >= NUM_OF_ROOM) {
        fprintf(stderr, "Invalid room_id: %d\n", room_id);
        exit(1);
    }

    Room *room = &rooms[room_id];
    return room->state;
}

void check_room_sockfd(Room *rooms, int room_id, int shm_id) {
    printf("check_room_sockfd %d\n", room_id);
    int flag = 0;
    Room *room = get_room_and_lock(rooms, room_id);
    for(int i=0; i<2; i++) {
        // 実際に通信をしてsockfdが有効かを確認
        if(room->players[i].sockfd != -2) {
            struct pollfd pfd;
            pfd.fd = room->players[i].sockfd;
            pfd.events = POLLIN | POLLPRI;
            
            int ret = poll(&pfd, 1, 0); // 0: timeout, 1: data available, -1: error
            if(ret == -1 || pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                flag = 1;
                break;
            }
        }
    }
    unlock_room(room_id);

    if(flag == 1) {
        init_room(room_id, shm_id);
    }
    printf("check_room_sockfd end\n");

}

