#include "client.h"

WSADATA initialize() {
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
        perror("WSAStartup");
        exit(1);
    }
    system("cls");
    return wsaData;
}

void finalize() {
    WSACleanup();
    exit(0);
}
