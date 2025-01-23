#include "server.h"

int room_making(int new_sockfd) {
    Room *rooms = attach_rooms();
    int room_id;

    for (int i = 1; i < NUM_OF_ROOM; i++) {
        check_room_sockfd(rooms, i); // ついでにソケットが閉じられている部屋を初期化
        if(get_room_state_no_lock(rooms, i) != EMPTY) { // lockしないでいったん確認，その後lockして再度確認してから処理（性能の低下を防ぐ）
            continue;
        }
        Room *room = get_room_and_lock(rooms, i);
        if (room->state == EMPTY) {
            printf("room_making %d\n", i);
            room->state = WAITING_FRIEND;
            room->players[0].sockfd = new_sockfd;
            room_id = i;
            printf("room_id: %d\n", room_id);
            char buf[30];
            sprintf(buf, "room_id %d", room_id);
            printf("send from room_making:%s\n", buf);
            send(room->players[0].sockfd, buf, 30, 0);
            unlock_room(i);
            break;
        }
        unlock_room(i);
    }
    detach_rooms(rooms);
    return room_id;
}

int room_searching(int room_id, int new_sockfd) {
    printf("room_searching %d\n", room_id);
    Room *rooms = attach_rooms();
    if(check_room_sockfd(rooms, room_id) == 1) {
        detach_rooms(rooms);
        send(new_sockfd, "failed", 6, 0);
        return -1;
    } // 閉じられていた場合は初期化
    Room *room = get_room_and_lock(rooms, room_id);
    if(room->state == WAITING_FRIEND) {
        room->state = PLAYING;
        room->players[1].sockfd = new_sockfd;
        int sockfd = room->players[0].sockfd;
        unlock_room(room_id);
        int len_1 = send(sockfd, "start", 5, 0);
        int len_2 = send(new_sockfd, "start", 5, 0);
        if(len_1 == -1 || len_2 == -1) {
            perror("send");
            init_room(room_id);
            return -1;
        }
        if(len_1 == 0 || len_2 == 0) {
            printf("connection closed:: room_searching\n");
            init_room(room_id);
            return -1;
        }
        printf("start %d\n", room_id);
        detach_rooms(rooms);
        battle(room_id);
        return 1;
    }
    else {
        unlock_room(room_id);
        send(new_sockfd, "failed", 6, 0);
        close(new_sockfd);
        return -1;
    }
}

