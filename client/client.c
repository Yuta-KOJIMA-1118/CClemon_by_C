#include "client.h"

// グローバル変数
Skill skills[5];

int main() {
    initialize();
    prepare_skills();
    output_home();
    while(1) {
        if(_kbhit()) {
            char c = _getch();
            if(c == '1') {
                printf("random matching\n");
                break; //todo remove
            } else if(c == '2') {
                printf("matching with friend\n");
                output_with_friend_menu();
                handle_with_friend_menu();
                break; //todo remove
            } else if(c == '3') {
                printf("matching with npc\n");
                break; //todo remove
            } else if(c == '4') {
                printf("exit\n");
                finalize();
            }
            else {
                printf("invalid input\n");
                printf("please type 1, 2, 3, 4\n");
            }
        }
    }
    finalize();
}
