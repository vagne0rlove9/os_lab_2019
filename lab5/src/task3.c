#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

static pthread_mutex_t mtx_first = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mtx_second = PTHREAD_MUTEX_INITIALIZER;


static void function1() {
    int i=0;
    printf("first function\n");
    pthread_mutex_lock(&mtx_first);
    do{
        i++;
    }while(1);
    pthread_mutex_lock(&mtx_second);
    do{
        i--;
    }while(1);
    pthread_mutex_unlock(&mtx_second);
    pthread_mutex_unlock(&mtx_first);
    printf("first function exit\n");
}

static void function2() {
    int i=0;
    printf("second function\n");
    pthread_mutex_lock(&mtx_second);
    do{
        i++;
    }while(i!=0);
    printf("second function increment done\n");
    pthread_mutex_lock(&mtx_first);
    printf("second function start decrement\n");
    do{
        i--;
    }while(1);
    pthread_mutex_unlock(&mtx_first);
    pthread_mutex_unlock(&mtx_second);
    printf("second function exit\n");
}

int main(int argc, char* argv[]) {
    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, (void*)function1, NULL)!=0) {
        printf("Error: cannot create first thread\n");
        return -1;
    }
    if (pthread_create(&t2, NULL, (void*)function2, NULL)!=0) {
        printf("Error: cannot create second thread\n");
        return -1;
    }

    if (pthread_join(t1, 0) != 0) {
        printf("Error: cannot join first thread\n");
        return -1;
    }

    if (pthread_join(t2, 0) != 0) {
        printf("Error: cannot join second thread\n");
        return -1;
    }
}

//gcc task3.c -lpthread -o task3.o && ./task3.o