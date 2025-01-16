#include "server.h"

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
            char buf[30];
            sprintf(buf, "room_id %d", room_id);
            printf("send:buf: %s\n", buf);
            send(room->players[0].sockfd, buf, 30, 0);
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
        printf("start %d\n", room_id);
        unlock_room(room_id);
        return 1;
    }
    else {
        unlock_room(room_id);
        return -1;
    }
}

