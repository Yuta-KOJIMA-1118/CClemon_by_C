#include "client.h"

void start_battle(int sockfd) {
    system("cls");
    // 3s 待つ
    printf("3...\n");
    Sleep(1000);
    printf("2...\n");
    Sleep(1000);
    printf("1...\n");
    Sleep(1000);

    send(sockfd, "start", 5, 0);

    printf("start_battle\n");
    int turn = 0;
    int e_lemon = -1;
    int y_lemon = -1;
    int e_skill = -1;
    int y_skill = -1;
    int e_prev_skill = -1;
    int y_prev_skill = -1;
    int selected_skill = 0;

    while(1) {
        int len;
        char buf[30];
        len = recv(sockfd, buf, 30, 0); // turn start signal
        if(len == 0) {
            printf("connection closed:: start_battle\n");
            finalize();
        }
        buf[len] = '\0';
        if(strcmp(buf, "turn start") != 0) {
            printf("invalid turn start signal\n");
            finalize();
        }
    
        pthread_t thread;
        if(pthread_create(&thread, NULL, pthread_battle_receiver, (void *)&sockfd) != 0) {
            perror("pthread_create");
            exit(1);
        }

        // handle_battle_menuで0.7s待つ
        output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, selected_skill, 0);
        handle_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, &selected_skill);

        BattleRcvData data;
        if(pthread_join(thread, (void *)&data) != 0) {
            perror("pthread_join");
            exit(1);
        }

        //todo バトル処理
    }




}