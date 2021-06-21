#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define LEN 1000000
int num = 0;

void* pthread_func(void* arg) {
    int i;
    int index = *(char*)arg - '0';
    for(i = 0; i < LEN; ++i) {
        num += 1;
        if(i>10000 && i<10100)
            printf("This is thread%d, now num = %d\n", index, num);
    }
}

int main() {
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, (void*)pthread_func, "1");
    pthread_create(&tid2, NULL, (void*)pthread_func, "2");
    
    char* rev = NULL;
    pthread_join(tid1, (void*)&rev);
    pthread_join(tid2, (void*)&rev);
    
    printf("correct result is: %d, wrong result is: %d\n", 2*LEN, num);
    return 0;
}
