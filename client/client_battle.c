#include "client.h"

void start_battle(int sockfd) {
    printf("start_battle\n");
    int turn = 0;
    int e_lemon = -1;
    int y_lemon = -1;
    int e_skill = -1;
    int y_skill = -1;
    int e_prev_skill = -1;
    int y_prev_skill = -1;

    while(1) {
        int len;
        char buf[30];
        len = recv(sockfd, buf, 30, 0); // turn start signal
        if(len == 0) {
            printf("connection closed:: start_battle\n");
            finalize();
        }
    
        pthread_t thread[2];
        if(pthread_create(&thread[0], NULL, pthread_battle_sender, (void *)&sockfd) != 0) {
            perror("pthread_create");
            exit(1);
        }
        if(pthread_create(&thread[1], NULL, pthread_battle_receiver, (void *)&sockfd) != 0) {
            perror("pthread_create");
            exit(1);
        }

        // turn 0
        output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn);
        // 0.35s
        Sleep(350);

        // turn 1
        turn++;
        output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn);
        // 0.35s
        Sleep(350);

        for(int i=0; i<2; i++) {
            if(pthread_join(thread[i], NULL) != 0) {
                perror("pthread_join");
                exit(1);
            }
        }

        //todo バトル処理

    }

    


}