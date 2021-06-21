#define _GNU_SOURCE 1
#include <stdlib.h>
#include <poll.h>
#include <errno.h>
#include "TcpServer.h"
#include <fcntl.h>

#define BUFFER_SIZE 64
#define CHECK_RET(p) if(p != true) return -1;

int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("usage: ./client [server ip] [server port]\n");
        return -1;
    }

    char* address = argv[1];
    uint16_t port = atoi(argv[2]);

    TcpServer client_sock;
    
    CHECK_RET(client_sock.CreateSocket());
    CHECK_RET(client_sock.Connect(address, port));

    pollfd fds[2];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = client_sock.getfd();
    fds[1].events = POLLIN | POLLRDHUP;
    fds[1].revents = 0;

    char read_buf[BUFFER_SIZE];
    int pipefd[2];
    int ret = pipe(pipefd);

    while(1) {
        ret = poll(fds, 2, -1);
        if(ret < 0) {
            printf("poll error.\n");
            break;
        }
        if(fds[1].revents & POLLRDHUP) {
            printf("server close the connect.\n");
            break;
        }
        else if(fds[1].revents & POLLIN) {
            memset(read_buf, '\0', BUFFER_SIZE);
            ret = recv(fds[1].fd, read_buf, BUFFER_SIZE-1, 0);
            printf("receive %d bytes message from server: %s\n",ret, read_buf);
        }
        if(fds[0].revents & POLLIN) {
            ret = splice(0, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
            ret = splice(pipefd[0], NULL, client_sock.getfd(), NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        }
    }
    close(client_sock.getfd());
    return 0;
}
