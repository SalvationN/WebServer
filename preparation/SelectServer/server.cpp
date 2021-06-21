#include "SelectServer.h"
#define CHECK_RET(p) if(p != true) {return -1;}

int main() {
    TcpServer listen_sock, accept_sock;
    CHECK_RET(listen_sock.CreateSocket());
    CHECK_RET(listen_sock.Bind("0.0.0.0", 12345));
    CHECK_RET(listen_sock.Listen());
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    char buffer[1024], ok[5] = "ok.";
    
    SelectServer server;
    server.addfd(listen_sock.getfd());

    while(1) {
        vector<TcpServer> vec;
        printf("a new circle.\n");
        if(!server.Select(vec))continue;
        printf("vector size: %d\n",vec.size());
        for(size_t i = 0; i<vec.size(); i++) {
            if(listen_sock.getfd() == vec[i].getfd()) {
                listen_sock.Accept(&client, accept_sock);
                
                printf("There is a new connection: %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                server.addfd(accept_sock.getfd());
            }
            else {
                printf("start recv msg.\n");
                if(!vec[i].Recv(buffer)) {
                    server.deletefd(vec[i].getfd());
                    vec[i].Close();
                    continue;
                }
                printf("receive message from client[%d]: %s\n", i, buffer);
                memset(buffer, '\0', 1024);
                vec[i].Send(ok);
            }
        }
    }
    return 0;
}
