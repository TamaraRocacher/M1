#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

/** Pour compiler : g++ -std=c++11 -o zoo tpnote.cpp -lpthread **/
/** Désolé pour la structure, je voulais à la base mettre verrou et cond dedans 
    mais ça ne marchait pas alors je les ai enlevé et je n'ai pas eu le temps d'enlever la structure **/


pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct zoo{
  int nbPlacesLibres;
  };

  void *bus(void* structure)
  {
    struct zoo *z=((struct zoo*)structure);
  int nbPlacesTotal=z->nbPlacesLibres;
    while(1)
      {
	pthread_mutex_lock(&verrou);
	while(z->nbPlacesLibres>0)
	  {
	    //  cout <<"Le bus se remplit..."<<endl;
	    pthread_mutex_unlock(&verrou);
	  }
	cout <<"Le bus est plein, le bus va faire une ronde du zoo... (sleep de 5 secondes)"<<endl;
	sleep(5);
	cout <<"Le bus a fini la ronde, les passagers vont descendre..."<<endl;
	z->nbPlacesLibres=nbPlacesTotal;
	
	pthread_cond_broadcast(&cond);//le bus est libre, les passagers vont pouvoir monter
	pthread_mutex_unlock(&verrou);
      }
  }

void *personne(void *structure)
{
  struct zoo *z=((struct zoo*)structure);

  bool estmonte=false;
  while(1)
    {
      pthread_mutex_lock(&verrou);
      if(z->nbPlacesLibres>0 && !estmonte)
	{
	  z->nbPlacesLibres--;
	  cout <<"Un passager monte dans le bus, il reste "<<z->nbPlacesLibres<<" places restantes..."<<endl;
	  estmonte=true;
	 
	  
	  
	}

      else
	{
	  cout <<"Le bus est plein, la personne doit attendre..."<<endl;
	   pthread_cond_wait(&cond, &verrou);
	   estmonte=false;
	}
    }
}


  
int main(int argc, char** argv)
{
  int nbPlacesBus=0;
  cout<<"Veuillez saisir le nombre de places dans le bus: "<<endl;
    cin>>nbPlacesBus;
  int nbPersonnes=0;
  cout <<"Veuillez saisir maintenant le nombre de personnes voulant rentrer dans le bus. Attention il doit y avoir plus de personnes que de places dans le bus. Si vous saisissez un nombre inférieur, il faudra retaper un nombre."<<endl;
  do
    {
      cout <<"Veuillez saisir le nombre de personnes:"<<endl;
      cin>>nbPersonnes;
    }
  while(nbPersonnes<nbPlacesBus);

 
  struct zoo z;
  
  z.nbPlacesLibres=nbPlacesBus;
  
  
   
     

 
  pthread_t *threads=(pthread_t *)malloc((nbPersonnes+1) * sizeof(pthread_t*));
 

    if(pthread_create(&threads[0],NULL, bus, &z)<0)
    {
      cout <<"Erreur lors de la création du thread du bus !"<<endl;
      exit(1);
    }
  
  for(int i=1; i<nbPersonnes+1; i++)
    {
   
    
      if(pthread_create(&threads[i], NULL, personne, &z)!=0)
	{
	  cout <<"Erreur lors de la création du thread d'une personne"<<endl;
	  exit(2);
	}
    }

  if(pthread_join(threads[0], NULL)<0)
    {
      cout <<"Erreur lors du join du thread du bus"<<endl;
      exit(3);
    }
  
   for(int i=1; i<nbPersonnes+1 ; i++)
    {
      if(pthread_join(threads[i], NULL)<0)
	{
	  cout <<"Erreur lors d'un join du thread d'une personne!"<<endl;
	  exit(4);
	}
    }
    
   return 0;
    
}
