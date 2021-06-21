#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

char* thread_func1(void* arg) {
    pid_t pid = getpid();   // get process id
    pthread_t tid = pthread_self();    // get pthread id
    printf("%s pid: %u, tid: %u (0x%x)\n", (char*)arg, (unsigned int)pid, (unsigned int)tid, (unsigned int)tid);
    char* msg = "thread_func1";
    return msg;
}

void* thread_func2(void* arg) {
    pid_t pid = getpid();
    pthread_t tid = pthread_self();
    printf("%s pid: %u, tid: %u (0x%x)\n", (char*)arg, (unsigned int)pid, (unsigned int)tid, (unsigned int)tid);
    char* msg = "thread_func2 ";
    while(1) {
        printf("%s is running\n", msg);
        sleep(1);   // sleep() is a system call, once it is exceuted, it will check if there is any cancel point.
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    if( pthread_create(&tid1, NULL, (void*)thread_func1, "new thread:") != 0 ) {
        printf("pthread_create error.\n");
        exit(EXIT_FAILURE);
    }

    if( pthread_create(&tid2, NULL, (void*)thread_func2, "new thread:") != 0 ) {
        printf("pthread_create error.\n");
        exit(EXIT_FAILURE);
    }
    
    char* recv = NULL;
    pthread_join(tid1, (void*)&recv);    // process will be blocked to wait for the end of the execution of tid1
    printf("%s return.\n", recv);
    pthread_cancel(tid2);

    printf("main thread end.\n");
    return 0;
}
