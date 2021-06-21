#include "TcpServer.h"
#include <stdlib.h>

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

    char buffer[1024];
    while(1) {
        printf("please input: ");
        scanf("%s", buffer);
        client_sock.Send(buffer);
        memset(buffer, '\0', 1024);

        if(!client_sock.Recv(buffer)) {
            printf("our socket has quit.\n");
            break;
        }
        printf("server said: %s\n", buffer);
    }
    client_sock.Close();
    return 0;
}
