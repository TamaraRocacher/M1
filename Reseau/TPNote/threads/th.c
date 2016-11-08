#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

static void* fn(void* z){
    int y = * ((int*) z);
    printf("Coucou %d\n",y);
    int x = 3;
    pthread_exit((void*)x);
}

int main() {
    pthread_t t;
    int z = 6;
    void *B=NULL;
    pthread_create(&t, NULL, fn,(void*) &z );
    pthread_join(t, &B);


    printf("%d\n", B);
    return 0;
}
