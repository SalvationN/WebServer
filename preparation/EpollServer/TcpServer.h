#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class TcpServer
{
    public:
        TcpServer():_sockfd(-1){}

        // socket
        bool CreateSocket() {
            int ret = socket(AF_INET, SOCK_STREAM, 0);
            if(ret < 0) {
                perror("socket error: ");
                return false;
            }
            _sockfd = ret;
            printf("socket succeed, the socket num is: %d\n", _sockfd); 
            int reuse = 0;
            setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
            return true;
        }

        // bind
        bool Bind(const char* ip, uint16_t port) {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            inet_pton(AF_INET, ip, &addr.sin_addr);

            int ret = bind(_sockfd, (struct sockaddr*)&addr, sizeof(addr));
            if(ret < 0) {
                perror("bind error: ");
                return false;
            }
            printf("bind succeed.\n"); 
            return true;
        }

        // listen
        bool Listen(int backlog = 5) {
            int ret = listen(_sockfd,backlog);
            if(ret < 0) {
                perror("listen error: ");
                return false;
            }
            printf("listen succeed.\n"); 
            return true;
        }

        // accept
        bool Accept(struct sockaddr_in* client, TcpServer& svr) {
            socklen_t addrlen = sizeof(struct sockaddr_in);
            int accfd = accept(_sockfd, (struct sockaddr*)client, &addrlen);
            if(accfd < 0) {
                perror("accept error: ");
                return false;
            }
            svr._sockfd = accfd;
            printf("accept succeed, the accept fd is: %d\n", accfd); 
            return true;
        }

        // connect
        bool Connect(const char* ip, uint16_t port) {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = inet_addr(ip);

            int ret = connect(_sockfd, (struct sockaddr*)&addr, sizeof(addr));
            if(ret < 0) {
                perror("connect error: ");
                return false;
            }
            printf("connect succeed.\n");
            return true;
        }
        
        // close
        void Close() {
            close(_sockfd);
            _sockfd = -1;
        }

        // set sockfd
        inline void setfd(int fd) {
            _sockfd = fd;
        }

        // get sockfd
        inline int getfd() {
            return _sockfd;
        }

        //send message
        bool Send(char* msg) {
            ssize_t ret = send(_sockfd, msg, strlen(msg), 0);
            if(ret == 0) {
                perror("send error: ");
                return false;
            }
            return true;
        }

        bool Recv(char* msg) {
            ssize_t ret = recv(_sockfd, msg, sizeof(msg), 0);
            if(ret < 0) {
                perror("recv error: ");
                return false;
            }
            return true;
        }
    private:
        int _sockfd;
};
