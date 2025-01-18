#include "client.h"

int request_room_making(int sockfd) {
    printf("request room making\n");
    char buf[30];
    sprintf(buf, "room_making 0\0");
    int len;
    send(sockfd, buf, strlen(buf), 0);
    len = recv(sockfd, buf, 30, 0);
    if(len == 0) {
        printf("connection closed\n");
        finalize();
    }
    buf[len] = '\0';
    printf("rbuf: %s\n", buf);
    //buf: room_id [room_id]
    char *label = strtok(buf, " ");
    char *data = strtok(NULL, " ");
    int room_id = atoi(data);
    return room_id;
}

void waiting_battle(int sockfd, int room_id) {
    printf("waiting battle\n");
    char buf[30];
    while(1) {
        int len = recv(sockfd, buf, 30, 0);
        buf[len] = '\0';
        printf("buf: %s\n", buf);
        if(len == 0) {
            printf("connection closed:: waiting_battle\n");
            finalize();
        }
        if(strcmp(buf, "start") == 0) {
            printf("battle start\n");
            start_battle(sockfd);
            return;
        }
    }
}


int request_room_searching(int sockfd, int n) {
    printf("request room searching\n");
    char buf[30];
    sprintf(buf, "room_searching %d", n);
    int len;
    printf("send: %s\n", buf);
    send(sockfd, buf, 30, 0);
    len = recv(sockfd, buf, 30, 0);
    if(len == 0) {
        printf("connection closed\n");
        finalize();
    }
    buf[len] = '\0';
    printf("recv in request_room_searching: %s\n", buf);
    if(strcmp(buf, "start") == 0) {
        printf("battle start\n");
        return 1;
    }
    else {
        return 0;
    }
}



int prepare_socket() {
    SOCKET sockfd;
    struct sockaddr_in serv;
    const char *ip = "127.0.0.1";
    int port = 12345;
    
    // ソケットの作成
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        perror("socket");
        exit(1);
    }

    // サーバー情報の設定
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = inet_addr(ip);

    // サーバーに接続
    if (connect(sockfd, (struct sockaddr *)&serv, sizeof(serv)) == SOCKET_ERROR) {
        perror("connect");
        closesocket(sockfd);
        exit(1);
    }

    return sockfd;
}
