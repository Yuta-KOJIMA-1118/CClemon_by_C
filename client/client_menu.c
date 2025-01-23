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

void handle_home() {
    int flag[6] = {0};
    while(1) {
        if(_kbhit()) {
            char c = _getch();
            if(c == '1') {
                if(flag[1] == 0) {
                    flag[1] = 1;
                    printf("random matching\n");
                    printf("未実装\n");
                }
            }
            else if(c == '2') {
                printf("matching with friend\n");
                output_with_friend_menu();
                handle_with_friend_menu();
                return;
            }
            else if(c == '3') {
                if(flag[3] == 0) {
                    flag[3] = 1;
                    printf("matching with npc\n");
                    printf("未実装\n");
                }
            }
            else if(c == '4') {
                printf("exit\n");
                finalize();
            }
            else {
                if(flag[5] == 0) {
                    flag[5] = 1;
                    printf("invalid input\n");
                    printf("please type 1, 2, 3, 4\n");
                }
            }
        }
    }
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
    return;
}

void output_battle_menu(int e_lemon, int y_lemon, int e_skill, int y_skill, int e_prev_skill, int p_prev_skill, int turn, int selected_skill, int mistake) {
    system("cls");
    printf("battle\n");
    printf("\n");

    printf("enemy's lemon: %d\n", e_lemon);
    if(e_prev_skill != -1) {
        printf("enemy's prev skill: %s\n", skills[e_prev_skill].name);
    }
    else {
        printf("\n");
    }
    printf("\n");

    switch(turn) {
        case 0:
            printf("enemy: C\n");
            printf("you:   C\n");
            break;
        
        case 1:
            printf("enemy: C C\n");
            printf("you:   C C\n");
            break;

        case 2:
            printf("enemy: C C %s\n", skills[e_skill].name);
            printf("you:   C C %s\n", skills[y_skill].name);
            break;
        
        default:
            printf("turn error %d\n", turn);
            break;
    }

    printf("\n");
    printf("your lemon: %d\n", y_lemon);

    if(p_prev_skill == -1) {
        printf("\n");
    }
    else {
        printf("your prev skill: %s\n", skills[p_prev_skill].name);
    }
    printf("\n");

    printf("select skill\n");
    printf("1. lemon\n");
    printf("2. fire\n");
    printf("3. barrier\n");
    printf("4. change\n");
    printf("5. gun\n");
    printf("\n");

    if(mistake) {
        printf("you don't have enough lemon\n");
    }
    printf("selected skill: %s\n", skills[selected_skill].name);
}

void handle_battle_menu(int e_lemon, int y_lemon, int e_skill, int y_skill, int e_prev_skill, int y_prev_skill, int *selected_skill) {
    const int interval_ms = 350;
    const int timeout_ms = 700;
    unsigned long start_time = GetTickCount();
    int mistake = 0;
    int turn = 0;

    while((GetTickCount() - start_time) < timeout_ms) {
        int input_received = 0;
        int cycle_start_time = GetTickCount();
        while((GetTickCount() - cycle_start_time) < interval_ms) {
            if(_kbhit()) {
                char c = _getch();
                switch(c) {
                    case '1':
                        printf("lemon\n");
                        *selected_skill = LEMON;
                        input_received = 1;
                        if(*selected_skill != LEMON) {
                            output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, *selected_skill, mistake);
                        }
                        break;
                    case '2':
                        printf("fire\n");
                        *selected_skill = FIRE;
                        input_received = 1;
                        if(y_lemon < skills[FIRE].energy && mistake == 0) {
                            mistake = 1;
                            *selected_skill = LEMON;
                            output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, *selected_skill, mistake);
                        }
                        else if(*selected_skill != FIRE) {
                            output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, *selected_skill, mistake);
                        }
                        break;
                    case '3':
                        printf("barrier\n");
                        *selected_skill = BARRIER;
                        input_received = 1;
                        if(*selected_skill != BARRIER) {
                            output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, *selected_skill, mistake);
                        }
                        break;
                    case '4':
                        printf("change\n");
                        *selected_skill = CHANGE;
                        input_received = 1;
                        if(y_lemon < skills[CHANGE].energy && mistake == 0) {
                            mistake = 1;
                            *selected_skill = LEMON;
                            output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, *selected_skill, mistake);
                        }
                        else if(*selected_skill != CHANGE) {
                            output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, *selected_skill, mistake);
                        }
                        break;
                    case '5':
                        *selected_skill = GUN;
                        printf("gun\n");
                        input_received = 1;
                        if(y_lemon < skills[GUN].energy && mistake == 0) {
                            mistake = 1;
                            *selected_skill = LEMON;
                            output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, *selected_skill, mistake);
                        }
                        else if(*selected_skill != GUN) {
                            output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, *selected_skill, mistake);
                        }
                        break;
                    default:
                        printf("invalid input\n");
                        break;
                }

            }
        }
        turn++;
        if(turn == 2) {
            break;
        }
        output_battle_menu(e_lemon, y_lemon, e_skill, y_skill, e_prev_skill, y_prev_skill, turn, *selected_skill, mistake);
    }
}

void output_result(int winner) {
    printf("\nresult\n");
    if(winner == 0) {
        printf("you lose\n");
    }
    else{
        printf("you win\n");
    }
    printf("\n");
    printf("type any key to return to home\n");
    _getch();
}


