#include "TcpServer.h"
#include <sys/epoll.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10
#define CHECK_RET(p) if(p != true) {return -1;}

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

// set epoll_ctl, enable_et indicates whether you expect to use ET mode
void addfd(int epollfd, int fd, bool enable_et) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if(enable_et) {
        event.events |= EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

// lt mode
void lt(epoll_event* events, int number, int epollfd, int listenfd) {
    printf("lt trigger once.\n");
    char buf[BUFFER_SIZE];
    for(int i = 0; i < number; i++) {
        int sockfd = events[i].data.fd;
        if(sockfd == listenfd) {
            struct sockaddr_in client_address;
            socklen_t client_addrlen = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlen);
            addfd(epollfd, connfd, false);
        }
        else if(events[i].events & EPOLLIN) {
            printf("event trigger once\n");
            memset(buf, '\0', BUFFER_SIZE);
            int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);
            if(ret < 0) {
                printf("recv error.\n");
                close(sockfd);
                continue;
            }
            printf("get %d byte message: %s\n", ret, buf);
        }
        else {
            printf("something else happened.\n");
        }
    }
}

// et mode
void et(epoll_event* events, int number, int epollfd, int listenfd) {
    char buf[BUFFER_SIZE];
    printf("et trigger once.\n");
    for(int i = 0; i < number; i++) {
        int sockfd = events[i].data.fd;
        if(sockfd == listenfd) {
            struct sockaddr_in client_address;
            socklen_t client_addrlen = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlen);
            addfd(epollfd, connfd, true);
        }
        else if(events[i].events & EPOLLIN) {
            printf("event trigger once.\n");
            while(1) {
                memset(buf, '\0', BUFFER_SIZE);
                int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);
                if(ret < 0) {
                    if(errno == EAGAIN || errno == EWOULDBLOCK) {
                        printf("read later.\n");
                        break;
                    }
                    close(sockfd);
                    break;
                }
                else if(ret == 0) {
                    close(sockfd);
                }
                else {
                    printf("get %d bytes message: %s\n", ret, buf);
                }
            }
        }
        else {
            printf("something else happend.\n");
        }
    }
}

int main(int argc, char* argv[]) {
    TcpServer listen_sock, accept_sock;
    CHECK_RET(listen_sock.CreateSocket());
    CHECK_RET(listen_sock.Bind("0.0.0.0", 12345));
    CHECK_RET(listen_sock.Listen());
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    
    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    if(epollfd == -1) {
        printf("epoll create error.\n");
        return -1;
    }
    addfd(epollfd, listen_sock.getfd(), true);

    while(1) {
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if(ret < 0) {
            printf("epoll error.\n");
            break;
        }
        et(events, ret, epollfd, listen_sock.getfd());
        // lt(events, ret, epollfd, listen_sock.getfd());
    }
    close(listen_sock.getfd());
    return 0;
}
