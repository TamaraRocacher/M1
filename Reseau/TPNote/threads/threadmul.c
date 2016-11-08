#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

/*int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    assert (end > 0L);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}*/




void* mul(void* par) {
	int* tab = (int*) par;
	int result = (intptr_t) malloc(sizeof(int));
	result=0;
	result = tab[0]*tab[1];
	printf("%d * %d = %d\n", tab[0],tab[1],result);
	pthread_exit((void*) (intptr_t) result);
}


int main(int argc, char** argv, char** env) {

	const int taille = atoi(argv[1]);
	int v1[taille], v2[taille], tmp[2];

	int r = 0, i=0;
	for(i=0;i<taille;i++) {
		v1[i] = 2;
		v2[i] = 2;
	}

	pthread_t tx[taille];
	void* tmpValue;

	int b = 0;
	for(i=0;i<taille;i++) {
		
		tmp[0]=v1[i];
		tmp[1]=v2[i];

		pthread_create(&tx[i],NULL, mul, (void*)tmp);
		pthread_join(tx[i], &tmpValue);
		b=  ((int) (intptr_t)tmpValue);
		r+= b;
		printf("r = %d et %d\n",r, (int)(intptr_t)tmpValue);
		//free(tmpValue);
	}

	printf("%d\n",r);

	/*for(i=0;i<taille;i++) {
		pthread_join(tx[i], &tmpValue);
		printf("\t%d\n", *((int*)tmpValue));
		r+= *((int*)tmpValue);
		free(tmpValue);
	}*/
	return 0;
}