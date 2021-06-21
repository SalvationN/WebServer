#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <stdio.h>
#include "TcpServer.h"
#include <vector>

using std::vector;

class SelectServer {
    public:
        SelectServer() {
            _maxfd = -1;
            FD_ZERO(&_readfds);
        }

        void addfd(int fd) {
            FD_SET(fd,&_readfds);
            if(fd > _maxfd){
                _maxfd = fd;
            }
        }

        void deletefd(int fd) {
            FD_CLR(fd,&_readfds);

            printf("delete fd: %d\n", fd);
            for(int i=_maxfd; i>=0; i--) {
                if(FD_ISSET(fd,&_readfds)) {
                    _maxfd = i;
                    break;
                }
            }
        }

        bool Select(std::vector<TcpServer>& vec) {
            struct timeval tv;
            tv.tv_sec = 20;
            tv.tv_usec = 0;

            fd_set tmp = _readfds;
            printf("start select.\n");
            int ret = select(_maxfd+1, &tmp, NULL, NULL, &tv);
            if(ret < 0) {
                perror("select error: ");
                return false;
            }
            else if(ret == 0) {
                printf("timeout!\n");
                return false;
            }

            for(int i=0; i<= _maxfd; i++) {
                if(FD_ISSET(i, &tmp)) {
                    TcpServer ts;
                    ts.setfd(i);
                    vec.push_back(ts);
                }
            }
            return true;
        }
    private:
        int _maxfd;
        fd_set _readfds;
};
