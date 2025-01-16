#include "server.h"

void battle(int room_id, int shm_id) {
    //Room *rooms = attach_rooms(shm_id);
    while(1) {
        // 1ターン1.4s
        // スレッドを使って2プレイヤーから受信
        pthread_t thread[2];
        Battle battle[2];
        battle[0].room_id = room_id;
        battle[0].player_num = 0;
        battle[0].shm_id = shm_id;
        battle[1].room_id = room_id;
        battle[1].player_num = 1;
        battle[1].shm_id = shm_id;

        for(int i=0; i<2; i++) {
            if(pthread_create(&thread[i], NULL, pthread_battle_receiver, (void *)&battle[i]) != 0) {
                perror("pthread_create");
                exit(1);
            }
        }

        for(int i=0; i<2; i++) {
            if(pthread_join(thread[i], NULL) != 0) {
                perror("pthread_join");
                exit(1);
            }
        }

        //todo バトル処理
    }
}
