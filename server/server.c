#include "server.h"

// グローバル変数
Skill skills[5];
pthread_mutex_t room_mutexes[NUM_OF_ROOM];


int main() {
    prepare_skills();

    int shm_id;
    Room *rooms;
    prepare_shared_memory(&shm_id, &rooms);

    int sockfd;
    socklen_t sin_siz;
    sockfd = prepare_socket(&sin_siz);

    while(1) {
        int new_sockfd;
        struct sockaddr_in clnt;
        if((new_sockfd = accept(sockfd, (struct sockaddr *)&clnt, &sin_siz)) < 0) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        switch(pid) {
            case -1:
                perror("fork");
                break;

            case 0: // 子プロセス
                printf("connect from %s: %d\n", inet_ntoa(clnt.sin_addr), ntohs(clnt.sin_port));
                receiver(new_sockfd, shm_id);
                exit(0);

            default: // 親プロセス
                break;
        }
    }
    printf("server end\n");
    close(sockfd);
    shmdt(rooms);
    shmctl(shm_id, IPC_RMID, NULL);
    destroy_mutexes();
    return 0;
}