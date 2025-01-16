#include "server.h"

int prepare_socket(socklen_t *sin_siz) {
    int sockfd;
    struct sockaddr_in serv;
    char *ip = "127.0.0.1";
    int port = 12345;
    
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    printf("socket() called\n");

    serv.sin_family = PF_INET;
    serv.sin_port = htons(port);
    inet_aton(ip, &serv.sin_addr);
    *sin_siz = sizeof(struct sockaddr_in);
    if(bind(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("bind");
        exit(1);
    }
    printf("bind() called\n");

    if(listen(sockfd, SOMAXCONN) < 0) {
        perror("listen");
        exit(1);
    }
    printf("listen() called\n");

    return sockfd;
}

void receiver(int new_sockfd, int shm_id) {
    char buf[100];
    memset(buf, 0, sizeof(buf));
    int len = recv(new_sockfd, buf, BUFSIZ, 0);
    if(len == 0) {
        printf("connection closed:: receiver\n");
        exit(1);
    }


    buf[len] = '\0';
    printf("received: %s\n", buf);

    // buf: [label] [data]
    char *label = strtok(buf, " ");
    char *data = strtok(NULL, " ");

    if(strcmp(label, "room_making") == 0) {
        int room_id = room_making(shm_id, new_sockfd);
        printf("room_id: %d\n", room_id);
    }
    else if(strcmp(label, "room_searching") == 0) {
        int room_id = atoi(data);
        room_searching(shm_id, room_id, new_sockfd);
    }
    else {
        printf("unknown label\n");
    }
}