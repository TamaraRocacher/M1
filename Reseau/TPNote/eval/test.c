#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/types.h>
#include<unistd.h>

typedef struct elements{int nbplacesDispo; int nbP; pthread_mutex_t verrou;
  pthread_cond_t cond1; pthread_cond_t cond2;int montee;}elements;//structure des var partagées
typedef struct param{int numth;elements elm;}param;


void * Bus(void*arg)//exécuté par le bus
{
  param * p = (param *)arg;
  // elements *elm = p -> elm;
  pthread_mutex_lock(&(p->elm.verrou));
  if(p->elm.montee == 1)//cas de la montée
    {
      while(p->elm.nbplacesDispo >0) {
	pthread_cond_wait(&(p->elm.cond1),&(p->elm.verrou));
	
      }
      pthread_mutex_unlock(&(p->elm.verrou));
      printf("le bus est plein allons-y\n");
      pthread_mutex_lock(&(p->elm.verrou));
      p->elm.montee = 0;
      pthread_mutex_unlock(&(p->elm.verrou));
      
      
    }
  else//cas descente
    {while(p->elm.nbplacesDispo <0) {
	pthread_cond_wait(&(p->elm.cond2),&(p->elm.verrou));
	
      }
      pthread_mutex_unlock(&(p->elm.verrou));
      printf("le bus est maintenant vide\n");
  pthread_mutex_lock(&(p->elm.verrou));
  p->elm.montee = 1;
  pthread_mutex_unlock(&(p->elm.verrou));

  
    }
  
  pthread_exit(NULL);			       
}


void*Passager(void*arg){
  param * p = (param *)arg;
  //elements *elm = p->elm;
  pthread_mutex_lock(&(p->elm.verrou));
  if(p->elm.montee == 1){//cas montee
    printf("un passager de plus\n");
    p->elm.nbplacesDispo --;
    if(p-> elm.nbplacesDispo>0)pthread_cond_wait(&(p->elm.cond1),&(p->elm.verrou));
    else 
      {pthread_cond_broadcast(&(p->elm.cond1));}
  }
  
  else{//cas descente
    printf("un passager de moins\n");
    p->elm.nbplacesDispo ++;
    if( p->elm.nbplacesDispo< 0  )pthread_cond_wait(&(p->elm.cond2),&(p->elm.verrou));
    else 
      {pthread_cond_broadcast(&(p->elm.cond2));}
  }
  
  pthread_exit(NULL);
}







int main(int argc, char*argv[]){
  if(argc != 3){printf("arguments requis (N,NbP)\n");exit(-1);}
  elements varP;
  param tabP[atoi(argv[2])];
  pthread_t tab[atoi(argv[2]) + 1];
  int i;
  //initialisation des vars
  varP.nbplacesDispo=atoi(argv[1]);
  varP.montee=1;
  pthread_cond_init(&(varP.cond1),NULL);
  pthread_cond_init(&(varP.cond2),NULL);
  pthread_mutex_init(&(varP.verrou),NULL);
  
  //creation threads
  tabP[0].numth=0;tabP[0].elm= varP;
  if(pthread_create(&tab[0],NULL, Bus,&(tabP[i])) != 0)
    {printf("erreur create bus\n");exit(-1);}
  
  
  for(i=1;i < atoi(argv[2]) + 1 ;i++){
    tabP[i].numth=i;
    tabP[i].elm= varP;
    if(pthread_create(&tab[i],NULL, Passager,&(tabP[i])) != 0)
      {printf("erreur create passager\n");exit(-1);}
    
  }
  
  //attente fin des threads
  
  if(pthread_join(tab[0],NULL) != 0){printf("erreur join B\n"); exit(-1);}
  
  for(i=1;i < atoi(argv[2]) + 1 ;i++)
    {if(pthread_join(tab[i],NULL) != 0){printf("erreur join P\n"); exit(-1);}
      
      //destruction verrou et conds
      
      pthread_mutex_destroy(&(varP.verrou));
      pthread_cond_destroy(&(varP.cond1));
      pthread_cond_destroy(&(varP.cond2));
    }
      
  return 0;
} 
  
