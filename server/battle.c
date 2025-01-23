#include "server.h"

void battle(int room_id) {
    Room *rooms = attach_rooms();
    Room *room = get_room_and_lock(rooms, room_id);
    int sockfd[2];
    sockfd[0] = room->players[0].sockfd;
    sockfd[1] = room->players[1].sockfd;
    printf("sockfd[0]: %d, sockfd[1]: %d\n", sockfd[0], sockfd[1]);
    unlock_room(room_id);

    char buf_1[30];
    char buf_2[30];
    int len_1;
    int len_2;

    len_1 = recv(sockfd[0], buf_1, 30, 0);
    len_2 = recv(sockfd[1], buf_2, 30, 0);
    if(len_1 == -1 || len_2 == -1 || len_1 == 0 || len_2 == 0) {
        printf("sockfd[0]: %d, sockfd[1]: %d\n", sockfd[0], sockfd[1]);
        perror("recv");
        unlock_room(room_id);
        init_room(room_id);
        exit(1);
    }
    printf("received in battle: %s, %s\n", buf_1, buf_2);

    if(len_1 == 0 || len_2 == 0) {
        printf("connection closed:: battle\n");
        //todo close
        exit(1);
    }

    buf_1[len_1] = '\0';
    buf_2[len_2] = '\0';

    if(strcmp(buf_1, "start") != 0 || strcmp(buf_2, "start") != 0) {
        printf("invalid start signal\n");
        unlock_room(room_id);
        init_room(room_id);
        exit(1);
    }

    // 1s wait
    printf("0.5s wait start\n");
    sleep(1);
    printf("0.5s wait end\n");

    while(1) {
        // 1ターン1.4s
        // スレッドを使って2プレイヤーから受信
        pthread_t thread[2];
        Battle battle[2];
        battle[0].room_id = room_id;
        battle[0].player_num = 0;
        battle[1].room_id = room_id;
        battle[1].player_num = 1;
        
        //現在時刻
        struct timeval start_tv;
        gettimeofday(&start_tv, NULL);

        for(int i=0; i<2; i++) {
            if(pthread_create(&thread[i], NULL, pthread_battle_receiver, (void *)&battle[i]) != 0) {
                perror("pthread_create");
                exit(1);
            }
        }

        // turn start signal
        printf("turn start signal\n");
        int len_1 = send(sockfd[0], "turn start", 30, 0);
        int len_2 = send(sockfd[1], "turn start", 30, 0);
        if(len_1 == -1 || len_2 == -1) {
            perror("send");
            unlock_room(room_id);
            init_room(room_id);
            exit(1);
        }
        if(len_1 == 0 || len_2 == 0) {
            printf("connection closed:: battle\n");
            unlock_room(room_id);
            init_room(room_id);
            exit(1);
        }

        for(int i=0; i<2; i++) {
            if(pthread_join(thread[i], NULL) != 0) {
                perror("pthread_join");
                unlock_room(room_id);
                init_room(room_id);
                exit(1);
            }
        }
        // ここまでで1.0s

        //バトル処理
        Room *room = get_room_and_lock(rooms, room_id);
        int skill[2];
        skill[0] = room->players[0].next_skill;
        skill[1] = room->players[1].next_skill;

        // check num_of_lemon to use skill, if not enough, use lemon
        for(int i=0; i<2; i++) {
            if(room->players[i].num_of_lemon < skills[skill[i]].energy) {
                skill[i] = LEMON;
            }
        }

        int who_win = -1; // 0:player0, 1:player1, -1:no winner

        // 本来は，priorityとtypeによって勝敗を決める
        // 面倒なので，べた書きする．

        for(int i=0; i<2; i++) {
            switch(skill[i]) {
                case LEMON:
                    room->players[i].num_of_lemon-= skills[LEMON].energy;
                    break;
                case FIRE:
                    room->players[i].num_of_lemon -= skills[FIRE].energy;
                    if(skill[(1+i)%2] == LEMON) {
                        who_win = i;
                    }
                    if(skill[(1+i)%2] == CHANGE) {
                        who_win = (1+i)%2;
                    }
                    break;
                case BARRIER:
                    room->players[i].num_of_lemon -= skills[BARRIER].energy;
                    break;
                case CHANGE:
                    room->players[i].num_of_lemon -= skills[CHANGE].energy;
                    break;
                case GUN:
                    room->players[i].num_of_lemon -= skills[GUN].energy;
                    if(skill[(1+i)%2] == LEMON || skill[(1+i)%2] == FIRE || skill[(1+i)%2] == BARRIER) {
                        who_win = i;
                    }
                    break;
                default:
                    printf("invalid skill\n");
                    unlock_room(room_id);
                    init_room(room_id);
                    exit(1);
            }
        }

        //send result
        char result_0[30], result_1[30];
        int winner_0 = -1, winner_1 = -1;
        if(who_win == 0) {
            winner_0 = 1;
            winner_1 = 0;
        }
        else if(who_win == 1) {
            winner_0 = 0;
            winner_1 = 1;
        }
        else {
            winner_0 = -1;
            winner_1 = -1;
        }
        // result <enemy's skill> <enemy's lemon> <your skill> <your lemon> <winner>
        sprintf(result_0, "result %d %d %d %d %d", skill[1], room->players[1].num_of_lemon, skill[0], room->players[0].num_of_lemon, winner_0);
        sprintf(result_1, "result %d %d %d %d %d", skill[0], room->players[0].num_of_lemon, skill[1], room->players[1].num_of_lemon, winner_1);
        
        // next_skillを初期化
        room->players[0].next_skill = 0;
        room->players[1].next_skill = 0;

        unlock_room(room_id);

        printf("send %s\n",result_0);
        len_1 = send(sockfd[0], result_0, 30, 0);
        len_2 = send(sockfd[1], result_1, 30, 0);
        if(len_1 == -1 || len_2 == -1) {
            perror("send");
            init_room(room_id);
            exit(1);
        }
        if(len_1 == 0 || len_2 == 0) {
            printf("connection closed:: battle\n");
            init_room(room_id);
            exit(1);
        }

        if(who_win != -1) {
            break;
        }

        // 1.4s になるようにwait
        struct timeval end_tv;
        gettimeofday(&end_tv, NULL);
        long diff = (end_tv.tv_sec - start_tv.tv_sec) * 1000000 + (end_tv.tv_usec - start_tv.tv_usec);
        long wait = 1400000 - diff;
        if(wait > 0) {
            usleep(wait);
        }        
    }
    detach_rooms(rooms);
    init_room(room_id);
    exit(0);
}
