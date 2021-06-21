#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <errno.h>

#define NUM 3

pthread_cond_t condv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void producer(void* arg) {
    int n = NUM;
    while(n--) {
        sleep(1);
        pthread_cond_signal(&condv);
        pthread_cond_signal(&condv);
        printf("producer thread send 2 notify signal.\n");
    }
}

void consumer(void* arg) {
    int n = 0;
    while(1) {
        pthread_cond_wait(&condv, &mutex);
        printf("receive producer notify signal. There is/are %d now.\n", ++n);
        if(n == NUM*2) {
            break;
        }
    }
}

int main() {
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, (void*)producer, NULL);
    pthread_create(&tid2, NULL, (void*)consumer, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}
