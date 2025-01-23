#include "server.h"

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

void receiver(int new_sockfd) {
    char buf[100];
    memset(buf, 0, sizeof(buf));
    int len = recv(new_sockfd, buf, BUFSIZ, 0);
    if(len == 0 || len == -1) {
        printf("connection closed:: receiver\n");
        exit(1);
    }


    buf[len] = '\0';
    printf("received in receiver: %s\n", buf);

    // buf: [label] [data]
    char *label = strtok(buf, " ");
    char *data = strtok(NULL, " ");

    if(strcmp(label, "room_making") == 0) {
        printf("room_making\n");
        room_making(new_sockfd);
    }
    else if(strcmp(label, "room_searching") == 0) {
        printf("room_searching\n");
        int room_id = atoi(data);
        room_searching(room_id, new_sockfd);
    }
    else {
        printf("unknown label\n");
        close(new_sockfd);
    }
}

void battle_receiver(int room_id, int player_num) {
    // 1.0s
    char buf[30];
    Room *rooms = attach_rooms();

    while(1) {
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(rooms[room_id].players[player_num].sockfd, &read_fds);

        int ret = select(rooms[room_id].players[player_num].sockfd + 1, &read_fds, NULL, NULL, &tv);
        if(ret == -1) {
            perror("select");
            init_room(room_id);
            exit(1);
        }
        else if(ret == 0) {
            printf("timeout\n");
            Room *room = get_room_and_lock(rooms, room_id);
            room->players[player_num].next_skill = 0; // lemon
            unlock_room(room_id);
            return;
        }

        if(FD_ISSET(rooms[room_id].players[player_num].sockfd, &read_fds)) {
            int len = recv(rooms[room_id].players[player_num].sockfd, buf, 30, 0);
            if(len == 0 || len == -1) {
                init_room(room_id);
                printf("connection closed:: battle_receiver\n");
                exit(1);
            }
            buf[len] = '\0';
            printf("received in battle_receiver: %s\n", buf);
            char *label = strtok(buf, " ");
            char *data = strtok(NULL, " ");

            if(strcmp(label, "skill") == 0) {
                int skill_id = atoi(data);
                Room *room = get_room_and_lock(rooms, room_id);
                room->players[player_num].next_skill = skill_id;
                unlock_room(room_id);
            }
            else {
                printf("unknown label\n");
            }
            break;
        }

    }
}


/*
void battle_receiver(int room_id, int player_num) {
    char buf[30];
    Room *rooms = attach_rooms();

    // ソケットを非ブロッキングモードに設定
    int sockfd = rooms[room_id].players[player_num].sockfd;
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    while (1) {
        int len = recv(sockfd, buf, 30, 0);

        if (len > 0) { // データ受信成功
            buf[len] = '\0';
            printf("received in battle_receiver: %s\n", buf);
            char *label = strtok(buf, " ");
            char *data = strtok(NULL, " ");

            if (strcmp(label, "skill") == 0) {
                int skill_id = atoi(data);
                Room *room = get_room_and_lock(rooms, room_id);
                room->players[player_num].next_skill = skill_id;
                unlock_room(room_id);
            } else {
                printf("unknown label\n");
            }
            break;
        } else if (len == 0) { // 接続が切断された
            printf("connection closed:: battle_receiver\n");
            init_room(room_id);
            exit(1);
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { // データがまだ到着していない場合
                //printf("waiting for data...\n");
                usleep(100000); // 0.1秒待機して再試行
            } else { // その他のエラー
                perror("recv");
                init_room(room_id);
                exit(1);
            }
        }

        // タイムアウト判定
        static int elapsed_time = 0;
        elapsed_time += 100;
        if (elapsed_time >= 1000) {
            printf("timeout\n");
            Room *room = get_room_and_lock(rooms, room_id);
            room->players[player_num].next_skill = 0; // lemon
            unlock_room(room_id);
            return;
        }
    }
}
*/

void *pthread_battle_receiver(void *arg) {
    Battle *battle = (Battle *)arg;
    battle_receiver(battle->room_id, battle->player_num);
    return NULL;
}
