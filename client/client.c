#include "client.h"

// グローバル変数
Skill skills[5];

int main() {
    initialize();
    prepare_skills();
    while(1) {
        output_home();
        handle_home();
    }
    finalize();
}
