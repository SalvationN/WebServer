#define _GNU_SOUTCE 1
#include "TcpServer.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMIT 65535
#define CHECK_RET(p) if(p != true) {return -1;}

struct client_data {
    sockaddr_in address;
    char* write_buf;
    char buf[BUFFER_SIZE];
};


int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

int main() {
    TcpServer listen_sock, accept_sock;
    CHECK_RET(listen_sock.CreateSocket());
    CHECK_RET(listen_sock.Bind("0.0.0.0", 12345));
    CHECK_RET(listen_sock.Listen());
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    client_data* users = new client_data[FD_LIMIT];
    pollfd fds[USER_LIMIT+1];
    int user_cnt = 0;
    for(int i = 0; i <= USER_LIMIT; i++) {
        fds[i].fd = -1;
        fds[i].events = 0;
    }
    fds[0].fd = listen_sock.getfd();
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;

    while(1) {
        int ret = poll(fds, user_cnt+1, -1);
        if(ret < 0) {
            printf("poll error.\n");
            break;
        }

        for(int i = 0; i < user_cnt + 1; i++) {
            if(fds[i].fd == listen_sock.getfd() && (fds[i].revents & POLLIN)) {
                int connfd = accept(listen_sock.getfd(), (struct sockaddr*)&client, &client_len);
                if(connfd < 0) {
                    perror("accept error: ");
                    continue;
                }
                if(user_cnt >= USER_LIMIT) {
                    const char* info = "too many users!\n";
                    printf("%s", info);
                    send(connfd, info, strlen(info), 0);
                    close(connfd);
                    continue;
                }
                user_cnt++;
                users[connfd].address = client;
                setnonblocking(connfd);
                fds[user_cnt].fd = connfd;
                // POLLRDHUP is used to judge whether client is closed, which helps server to avoid regarding closing signal as a read event.
                fds[user_cnt].events = POLLIN | POLLRDHUP | POLLERR;
                fds[user_cnt].revents = 0;
                printf("comes a new user, now have %d users\n", user_cnt);
            }
            else if(fds[i].revents & POLLERR) {
                printf("get an error from %d\n", fds[i].fd);
                char errors[100];
                memset(errors, '\0', 100);
                socklen_t len = sizeof(errors);
                if(getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, &errors, &len) < 0) {
                    printf("get socket option failed\n");
                }
                continue;
            }
            else if(fds[i].revents & POLLRDHUP) {
                users[fds[i].fd] = users[fds[user_cnt].fd];
                fds[i].fd = fds[user_cnt].fd;
                user_cnt--;
                i--;
                printf("a client left.\n");
            }
            else if(fds[i].revents & POLLIN) {
                int connfd = fds[i].fd;
                memset(users[connfd].buf, '\0', BUFFER_SIZE);
                ret = recv(connfd, users[connfd].buf, BUFFER_SIZE-1, 0);
                printf("get %d bytes of client data from %d: %s\n", ret, connfd, users[connfd].buf);
                if(ret < 0) {
                    if(errno != EAGAIN) {
                        close(connfd);
                        users[fds[i].fd] = users[fds[user_cnt].fd];
                        fds[i] = fds[user_cnt];
                        user_cnt--;
                        i--;
                    }   
                }
                else if(ret == 0){
                }
                else {
                    for(int j = 1; j <= user_cnt; j++) {
                        if(fds[j].fd == connfd) {
                            continue;
                        }
                        fds[j].events |= ~POLLIN;
                        fds[j].events |= POLLOUT;
                        users[fds[j].fd].write_buf = users[connfd].buf;
                    }
                }
            }
            else if(fds[i].revents & POLLOUT) {
                int connfd = fds[i].fd;
                if(!users[connfd].write_buf) {
                    continue;
                }
                printf("%s", users[connfd].write_buf);
                ret = send(connfd, users[connfd].write_buf, strlen(users[connfd].write_buf), 0);
                users[connfd].write_buf = NULL;
                fds[i].events |= ~POLLOUT;
                fds[i].events |= POLLIN;
            }
        }
    }
    delete [] users;
    close(listen_sock.getfd());
    return 0;
}
