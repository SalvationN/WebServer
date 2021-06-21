#include <unistd.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <stdio.h>
#include "TcpServer.h"
#include <vector>

#define MAX_OPEN 10

using std::vector;

class PollServer {
    public:
        PollServer() {
            MaxfdIndex = -1;
            for(int i = 0; i < MAX_OPEN; i++) {
                fds[i].fd = -1;
            }
        }

        int addfd(int fd) {
            int i;
            for(i = 0; i < MAX_OPEN; i++) {
                if(fds[i].fd == -1) {
                    fds[i].fd = fd;
                    break;
                }
            }
            if(i == MAX_OPEN) {
                printf("The number of fd exceeds the limit. MAX_OPEN is %d.\n", MAX_OPEN);
                return -1;
            }
            for(int i = MAX_OPEN - 1; i >= 0; i--) {
                if(fd[i].fd != -1) { 
                    MaxfdIndex = i;
                    break;
                }
            }
            return 0;
        }

        int deletefd(int fd) {
            int i;
            for(i = 0; i < MAX_OPEN; i++) {
                if(fds[i].fd == fd) {
                    fds[i].fd = -1;
                    printf("delete fd: %d\n", fd);
                    break;
                }
            }
            if(i == MAX_OPEN) {
                printf("There is no fd: %d.\n", fd);
                return -1;
            }
            return 0;
        }

        bool Poll() {
            int ret = poll(fds, MaxfdIndex, -1);
            if(ret < 0) {
                perror("poll error: ");
                return false;
            }
            return true;
        }
    private:
        int MaxfdIndex;
        struct pollfd fds[OPEN_MAX];
};
