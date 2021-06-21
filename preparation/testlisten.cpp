#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static bool stop = false;
// a signal processing function, it must be static.
static void handle_term( int sig ){
    stop = true;
    printf( "the function handle_term has been done.\n" );
}

int main(int argc, char* argv[]){
    // Question: How do this fuction work?
    signal( SIGTERM, handle_term );

    // This is a standard method to cope with missing args for a procedure. What you need to remember is function "basename", which means the path will be printed
    // both when the path contains ".", "/", or "..", and it will return the words after last '/'.
    if(argc<=3){
        printf( "usage: %s ip_address port_number backlog\n", basename(argv[0]));
        return 1;
    }
    int port = atoi(argv[2]);
    const char* ip = argv[1];
    int backlog = atoi(argv[3]);
    
    // This socket use IPv4 and it is stream-oriented(SOCK_STREAM).
    int sock = socket(PF_INET, SOCK_STREAM,0);
    assert(sock!=0);

    // sockaddr_in has 3 variables: address family(AF_INET), port number(net byte-order), IPv4 address(u_int32). 
    struct sockaddr_in address;
    bzero(&address, sizeof(address));       // It can be replaced by memset().
    address.sin_family = AF_INET;
    // This function is to change the address represented in dotted-decimal notation(e.g. 127.0.0.1) to net byte-order.
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);
    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    // perror can print the specific information about the errors.
    perror("bind: ");
    printf("%d\n",ret);
    assert(ret != -1);

    ret = listen(sock, backlog);
    assert(ret != -1);

    
    while(!stop){
        sleep(1);
    }

    close(sock);
    return 0;
}
