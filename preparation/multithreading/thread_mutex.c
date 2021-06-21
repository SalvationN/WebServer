#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define LEN 1000000
int num = 0;

void* pthread_func(void* arg) {
    int i;
    pthread_mutex_t* p_mutex = (pthread_mutex_t*)arg;
    for(i = 0; i < LEN; ++i) {
        pthread_mutex_lock(p_mutex);
        num += 1;
        pthread_mutex_unlock(p_mutex);
    }
}

int main() {
    pthread_mutex_t m_mutex;
    pthread_mutex_init(&m_mutex, NULL);

    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, (void*)pthread_func, (void*)&m_mutex);
    pthread_create(&tid2, NULL, (void*)pthread_func, (void*)&m_mutex);
    
    char* rev = NULL;
    pthread_join(tid1, (void*)&rev);
    pthread_join(tid2, (void*)&rev);

    printf("correct result is: %d, wrong result is: %d\n", 2*LEN, num);
    return 0;
}
