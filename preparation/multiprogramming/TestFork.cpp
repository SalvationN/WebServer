#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    pid_t fpid;
    printf("i son/pa ppid pid fpid\n");
    for(int i = 0; i < 2; i++) {
        fpid = fork();
        if(fpid == 0) {
            printf("%d child %d %d %d\n", i, getppid(), getpid(), fpid);
        } else {
            wait(NULL);
            printf("%d father %d %d %d\n", i, getppid(), getpid(), fpid);
        }
    }
    return 0;
}
