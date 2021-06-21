#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_PORT 12345

int main(int argc, char** argv) {
    int connfd = 0;
    int cLen = 0;
    struct sockaddr_in client;
    if(argc < 2) {
        printf("usage: ./TestSelectClient [server IP address.\n]");
        return -1;
    }
    client.sin_family = AF_INET;
    client.sin_port = htons(DEFAULT_PORT);
    client.sin_addr.s_addr = inet_addr(argv[1]);
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connfd < 0) {
        perror("socket error: ");
        return -1;
    }
    printf("socket ok.\n");

    if(connect(connfd, (struct sockaddr*)&client, sizeof(client)) < 0) {
        perror("connect error: ");
        return -1;
    }
    char buffer[1024];
    bzero(buffer,sizeof(buffer));
    recv(connfd, buffer, 1024, 0);
    printf("recv: %s", buffer);
    while(1) {
        bzero(buffer,sizeof(buffer));
        scanf("%s", buffer);
        int p = strlen(buffer);
        buffer[p] = '\0';
        send(connfd, buffer, 1024, 0);
        printf("send buffer!\n");
    }
    close(connfd);
    return 0;
}
