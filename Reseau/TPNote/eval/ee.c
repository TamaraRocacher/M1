#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>



typedef struct _Data {
    int* N; 
    int* NbP;
    pthread_cond_t* cond; 
    pthread_mutex_t* lock; 
} Data;


void* tache_bus(void* ptr){

    Data* data = (Data*)ptr;

    
    printf(" Debut de la tache du thread  bus \n");
  
   
    if(pthread_mutex_lock(data->lock) != 0){    
        perror("Erreur du lock");
        exit(-1);
    }
   
    if(*data->N <= 0){    // Si le counter est à 0 ça signifie que toutes les places du bus sont occupée
        printf(" toutes les places du bus sont occupée,le bus est plein \n");
	printf ("le bus démmare! \n");
	sleep(2);
	printf ("le bus est arrivé! je suis a l'arret! \n");
    	pthread_cond_wait(data->cond, data->lock);
    }
	 
// il reste des places dans le bus
    while(*data->N > 0){  
        printf("%d : Il reste des places dans le bus. \n", *data->N);
         
	 pthread_cond_broadcast(data->cond);  // Donc on peut lancer le révei
pthread_cond_wait(data->cond, data->lock);
    }

    if(pthread_mutex_unlock(data->lock) != 0){  
        perror("Erreur unlock");
        exit(-1);
    }
   
    printf(" Fin du voyage\n");
    pthread_exit(NULL);
free (ptr);
}



void* tache_passager(void* ptr){
  
    Data* data = (Data*)ptr;

  
    printf("%d : Debut du thread passagers \n", (int) pthread_self());
  
   
    if(pthread_mutex_lock(data->lock) != 0){   
      perror("Erreur du lock");
      exit(-1);
    }
    
    if(*data->N > 0){    
      printf("j'ai trouvé une place !\n");
      *data->NbP-= 1;
      *data->N-= 1;
    }
    else {
      printf("j'attends le prochain bus !\n");
      pthread_cond_wait(data->cond, data->lock);
    }
    
    printf("je suis dans le bus, je fais mon voyage !\n");
    sleep(2);
    printf("je descends du bus ");
    
    if(pthread_mutex_unlock(data->lock) != 0){ 
      perror("Erreur du unlock des passagers");
      exit(-1);
    }
    
    pthread_exit(NULL);
    free (ptr);
}





int main(){
while(1){
    // Entree par l'utilisateur du nombre de passagers (nombre de threads)
    int NbP; int N;
    printf("Veuillez entrer le nombre  de passagers  : \n");
    scanf("%d", &NbP);
//Entree par l'utili le nombre de place disponible pour un bus donné
    printf("Veuillez entrer le nombre  de places disponible pour votre bus  : \n");
    scanf("%d", &N);
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    //pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t uneCond;
    pthread_cond_init(&uneCond,NULL);
  
 Data *data;
    data = malloc(sizeof(Data)); 
    data->NbP = &NbP;
    data->N= &N;
    data->lock = &lock;
    data->cond = &uneCond;

//creation du thread bus 
    pthread_t thr_bus;
   
 if (pthread_create(&thr_bus, NULL,tache_bus, data)!=0)
{
perror("erreur de creation du thread bus");
}


//Creation des threads passagers
	pthread_t idth[NbP];
	

 // Creation des threads un par un
    for(int i = 0; i < NbP; i++){
               
        if(pthread_create(&(idth[i]), NULL,tache_passager,data) != 0){
            perror("Erreur de creation d'un thread passager: ");
        }
    }

    // Join de tous les threads un par un pour attendre la fin des traitements
    for(int i = 0; i < NbP; i++) {
        if (pthread_join(idth[i], NULL) != 0) {
            perror("Erreur de join de thread passagers : ");
        }
    }
// join du thread bus 
if (pthread_join(thr_bus, NULL) != 0) {
            perror("Erreur de join de thread bus : ");
        }

    // Nettoyage
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&uneCond);
 }  
}

