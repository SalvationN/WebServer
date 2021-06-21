#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define DEFAULT_PORT 12345

int main(int argc, char** argv) {
    int serverfd,acceptfd;
    struct sockaddr_in my_addr;
    struct sockaddr_in client_addr;
    unsigned int sin_size, myport=12345, lisnum=10;
    
    /* create listen socket  */
    if((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("There are some errors when creating socket: ");
        return -1;
    }
    printf("socket succeed.\n");

    // transfer the address
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEFAULT_PORT);
    // INADDR_ANY represents 0.0.0.0, that means all requests will be responsed by the IP
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 0);

    // bind
    if(bind(serverfd, (struct sockaddr*)(&my_addr), sizeof(struct sockaddr))==-1) {
        perror("bind error: ");
        return -1;
    }
    printf("bind succeed.\n");

    // listen, backlog=10
    if(listen(serverfd, lisnum) == -1) {
        perror("listen error: ");
        return -1;
    }
    printf("listen succeed.\n");

    fd_set client_fdset;
    int maxsock;
    struct timeval tv;
    int client_sockfd[5];
    bzero((void*)client_sockfd,sizeof(client_sockfd));
    int conn_amount = 0;
    maxsock = serverfd;
    char buffer[1024];
    int ret=0;

    while(1) {
        printf("A new circle start.\n");
        FD_ZERO(&client_fdset);
        FD_SET(serverfd, &client_fdset);
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        for(int i=0; i<5; ++i) {
            if(client_sockfd[i] != 0) {
                FD_SET(client_sockfd[i], &client_fdset);
            }
        }
        ret = select(maxsock+1, &client_fdset, NULL, NULL, &tv);
        if(ret < 0) {
            perror("select error: ");
            break;
        }
        else if(ret == 0) {
            printf("timeout!\n");
            continue;
        }
        
        for(int i=0; i<conn_amount; i++) {
            if(FD_ISSET(client_sockfd[i], &client_fdset)) {
                printf("start recv from client[%d]:\n", i);
                ret = recv(client_sockfd[i], buffer, 1024, 0);
                if(ret <= 0) {
                    printf("client[%d] closed.\n", i);
                    FD_CLR(client_sockfd[i], &client_fdset);
                    client_sockfd[i] = 0;
                }
                else {
                    printf("recv from client[%d]: %s\n", i, buffer);
                }
            }
        

        if(FD_ISSET(serverfd, &client_fdset)) {
            struct sockaddr_in client_addr;
            size_t size = sizeof(struct sockaddr_in);
            int sock_client = accept(serverfd, (struct sockaddr*)(&client_addr), (unsigned int*)(&size));
            if(sock_client < 0) {
                perror("accept error: ");
                continue;
            }
            if(conn_amount < 5) {
                client_sockfd[conn_amount++] = sock_client;
                bzero(buffer, 1024);
                strcpy(buffer, "This is server, welcome!\n");
                send(sock_client, buffer, sizeof(buffer), 0);
                printf("new connection client[%d] %s:%d\n", conn_amount, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                bzero(buffer, 1024);
                ret = recv(sock_client, buffer, 1024, 0);
                if(ret < 0) {
                    perror("recv error: ");
                    close(sock_client);
                    return -1;
                }
                if(sock_client > maxsock) {
                    maxsock = sock_client;
                }
                else {
                    printf("max connection! quit!!\n");
                    break;
                }
            }
        }
    }
    for(int i=0; i<5; i++) {
        if(client_sockfd[i] != 0) {
            close(client_sockfd[i]);
        }
    }
    close(serverfd);
    return 0;
}
