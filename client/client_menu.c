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

void output_battle_menu(int e_lemon, int y_lemon, int e_skill, int y_skill, int e_prev_skill, int p_prev_skill, int turn) {
    system("cls");
    printf("battle\n");
    printf("\n");

    printf("enemy's lemon: %d\n", e_lemon);
    printf("enemy's prev skill: %s\n", skills[e_prev_skill].name);
    
    switch(turn) {
        case 0:
            printf("C\n");
            printf("C\n");
            break;
        
        case 1:
            printf("C C\n");
            printf("C C\n");
            break;

        case 2:
            printf("C C %s\n", skills[e_skill].name);
            printf("C C %s\n", skills[y_skill].name);
            break;
        
        default:
            printf("turn error %d\n", turn);
            break;
    }

    printf("\n");
    printf("your lemon: %d\n", y_lemon);
    printf("your prev skill: %s\n", skills[p_prev_skill].name);
    printf("\n");

    printf("select skill\n");
    printf("0. lemon\n");
    printf("1. fire\n");
    printf("2. barrier\n");
    printf("3. change\n");
    printf("4. gun\n");
    printf("\n");
}
