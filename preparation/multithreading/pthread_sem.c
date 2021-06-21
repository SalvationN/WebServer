#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define NUM 5

int queue[NUM];
sem_t psem, csem;

void producer(void* arg) {
    int i, pos = 0;
    int num, cnt = 0;
    for(i = 0; i < 9; ++i) {
        num = rand() % 100;
        cnt += num;
        sem_wait(&psem);
        queue[pos] = num;
        sem_post(&csem);
        printf("produce: %d\n", num);
        pos = (pos + 1) % NUM;
        sleep(rand() % 2);
    }
    printf("producer totally produce %d\n", cnt);
}

void consumer(void* arg) {
    int i, pos = 0;
    int num, cnt = 0;
    for(i = 0; i < 9; ++i) {
        sem_wait(&csem);
        num = queue[pos];
        sem_post(&psem);
        printf("consumer: %d\n", num);
        cnt += num;
        pos = (pos + 1) % NUM;
        sleep(rand() % 4);
    }
    printf("consumer totally consume %d\n", cnt);
}

int main() {
    sem_init(&psem, 0, NUM);
    sem_init(&csem, 0 ,0);

    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, (void*)consumer, NULL);
    pthread_create(&tid2, NULL, (void*)producer, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
} 
