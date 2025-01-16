#include "client.h"

void output_home() {
    system("cls");
    printf("CClemon online\n");
    printf("\n");
    printf("select menu\n");
    printf("1. random matching\n");
    printf("2. matching with friend\n");
    printf("3. mathcing with npc\n");
    printf("4. exit\n");
    printf("\n");
}

void output_with_friend_menu() {
    printf("type 0 to room making\n");
    printf("type negative number to exit\n");
    printf("other number to room entering\n");
    printf("\n");
}

void handle_with_friend_menu() {
    int n;
    scanf("%d", &n);
    int room_id;
    if(n == 0) {
        int sockfd = prepare_socket();
        room_id = request_room_making(sockfd);
        printf("room_id = %d\n", room_id);
        waiting_battle(sockfd, room_id);
    }
    else if(n < 0) {
        printf("exit\n");
        finalize();
    }
    else {
        printf("searching %d room\n", n);
        room_id = n;
        int sockfd = prepare_socket();
        if(request_room_searching(sockfd, n)) {
            start_battle(sockfd);
        }
        else {
            printf("failed to start battle\n");
            return;
        }
    }
}
