#include "client.h"

void start_battle(int sockfd) {
    printf("start_battle\n");
    while(1) {
        if(_kbhit()) {
            char c = _getch();
            if(c == '1') {
                printf("charge\n");
                char buf[30];
                sprintf(buf, "skill 0\0");
                send(sockfd, buf, strlen(buf), 0);
            } else if(c == '2') {
                printf("attack\n");
                char buf[30];
                sprintf(buf, "skill 1\0");
                send(sockfd, buf, strlen(buf), 0);
            } else if(c == '3') {
                printf("defence\n");
                char buf[30];
                sprintf(buf, "skill 2\0");
                send(sockfd, buf, strlen(buf), 0);
            } else if(c == '4') {
                printf("reflect\n");
                char buf[30];
                sprintf(buf, "skill 3\0");
                send(sockfd, buf, strlen(buf), 0);
            } else {
                printf("invalid input\n");
                printf("please type 1, 2, 3, 4\n");
            }
        }
    }
}