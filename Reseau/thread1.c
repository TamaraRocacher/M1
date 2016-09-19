#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* f1(void* x){
  printf("Etape 1 de f1: thread 1 ... non fini!\n" );
  sleep(2);
  printf("Etape 2 de f1: thread 1 ... non fini!\n");
  sleep(3);
  printf("Etape 3 de f1: thread 1 ...  fini!\n\n");
    int b =2;

  pthread_exit((void*)b);
  //return x;
}

void* f2(){
  printf("Etape 1 de f2: thread 2... non fini!\n" );
  sleep(1);
  printf("Etape 2 de f2: thread 2 ... non fini!\n");
  printf("Etape 3 de f2: thread 2 ...  fini!\n\n");
  return 0;
}

void* f3(){
  printf("Etape 1 de f3: thread 3 ... non fini!\n" );
  sleep(4);
  printf("Etape 2 de f3: thread 3 ... non fini!\n");
  sleep(3);
  printf("Etape 3 de f3: thread 3 ...  fini!\n\n");
  return 0;
}

int main()
{
    pthread_t t1;
    /*pthread_t t2;
    pthread_t t3;*/
    int ret1, ret2, ret3;
    void *ret4 = NULL;
    void* ret5;
    void* ret6;
    int ret = 3;



    if((ret1 = pthread_create( &t1, NULL, f1, (void *) &ret)) !=0){
      printf("erreur!!!\n");
      return 1;
    }

    /*if((ret2 = pthread_create( &t2, NULL, f2, NULL)) !=0){
      printf("erreur!!!\n");
      return 1;
    }

    if((ret3 = pthread_create( &t1, NULL, f3, NULL)) !=0){
      printf("erreur!!!\n");
      return 1;
    }*/

    pthread_join(t1 ,((void**) &ret4));
    /*pthread_join(t2 , &ret5);
    pthread_join(t3 , &ret6);*/

    printf("retour f1: %d  \n\n", ret4);
    return 0;
}
