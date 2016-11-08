#include<stdio.h>
#include<pthread.h>
void* myprint(void *x)
{
        
     int k = *((int *)x);
      printf("\n Thread created.. value of k [%d]\n",k);
       k =11;
       // pthread_exit((void *)k);
       //
       //
    pthread_exit((void*)k);
}
int main()
{
        
     pthread_t th1;
      int x =5;
       int *y;
        pthread_create(&th1,NULL,myprint,(void*)&x);
         pthread_join(th1,(void*)&y);
          printf("\n Exit value is [%d]\n",*y);
    return 0;
}  
