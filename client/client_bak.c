// clientはWindows上で動く

#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // キーボードからEnter無しでの入力用

#include <winsock2.h> // Windows用ソケット リンクには -lws2_32 が必要
//gcc -o client client.c -lws2_32

WSADATA initialize();
void finalize();

void output_home();
void output_with_friend_menu();

void handle_with_friend_menu();

int request_room_making(int sockfd);
void waiting_battle(int sockfd, int room_id);
void request_room_searching(int sockfd, int n);

int prepare_socket();

int main() { // 済
    initialize();
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

WSADATA initialize() { // 済
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
        perror("WSAStartup");
        exit(1);
    }
    system("cls");
    return wsaData;
}

void finalize() { // 済
    WSACleanup();
    exit(0);
}


void output_home() { // 済
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

void output_with_friend_menu() { // 済
    printf("type 0 to room making\n");
    printf("type negative number to exit\n");
    printf("other number to room entering\n");
    printf("\n");
}

void handle_with_friend_menu() { // 済
    int n;
    scanf("%d", &n);
    printf("n = %d\n", n);
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
        request_room_searching(sockfd, n);
    }
}

int request_room_making(int sockfd) { // 済
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

void waiting_battle(int sockfd, int room_id) { // 済
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
            return;
        }
    }
}

void request_room_searching(int sockfd, int n) { // 済
    char *buf;
    sprintf(buf, "room_searching %d\0", n);
    int len;
    send(sockfd, buf, strlen(buf), 0);
    len = recv(sockfd, buf, len, 0);
    if(len == 0) {
        printf("connection closed\n");
        finalize();
    }
    buf[len] = '\0';

    //todo buf
}

int prepare_socket() { // 済
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

