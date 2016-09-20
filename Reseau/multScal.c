#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>


void* add (void* tab){
  int* tmp = (int*) tab;
	int res = (intptr_t )malloc(sizeof(int));
  res =0;
  int i=0;
  int taille = tmp[0];
  for(i=1;i<taille;i++){
    res = res + tmp[i];
    printf("res (tour %d) : %d\n",i,res );

  }
	printf("add = %d\n", res);
	pthread_exit((void*)(intptr_t )res);
}
void* multiplication (void* chiffres){
  int* tab = (int*) chiffres;
	int res =(intptr_t ) malloc(sizeof(int));
	res = tab[0]*tab[1];
	printf("%d * %d = %d\n", tab[0],tab[1],res);
	pthread_exit((void*)(intptr_t )res);
}

int main(){
  int i=0, j=2, taille=10, r=0;
  int vecteur1[taille];
  int vecteur2[taille];
  int aMult[2];
  int mult[taille+1];
  mult[0]=taille;
  int b=0,c=0;

  for(i=0; i<taille;i++){
    vecteur1[i]= j++;
    vecteur2[i]= j;
    j+=2;
  }
  void* tmpValue;
  void* rFin;
  pthread_t threads[taille];
  pthread_t addition;

  for(i=0; i<taille;i++){
    aMult[0] = vecteur1[i];
    aMult[1] = vecteur2[i];
    if(pthread_create(&threads[i],NULL,multiplication,(void*)aMult)!=0){
      printf("erreur lors de la création du thread!\n");
      return 1;
    }
    pthread_join(threads[i], &tmpValue);


    b = ((int)(intptr_t )tmpValue);
    /* //cas 1: addition dans le thread principal
    r = r + b;
    printf("%d\n",r);*/

    //cas 2: addition par un autre thread
    mult[i+1]=b; //(int*)tmpValue;
  }
  pthread_create(&addition,NULL,add,(void*)mult);
  pthread_join(addition,&rFin);
  c=((int)(intptr_t )rFin);
    printf("%d\n",c);
    return 0;
}
