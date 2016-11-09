#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// variables partagées gérant l'état du bus
struct etatBus {

  int pl_occupees;           // pour compter le nombre de passagers dans le bus
  int capacite;              // le nombre max de passagers acceptés dans le bus
  int etat;                  // variable initialisée à 0 pour
			     // déclancher la montée, mise à 1 pour
			     // déclancher la descente (fin du
			     // voyage), mise à 2, pendant le voyage
			     // (descente et montée non autorisée)
  pthread_mutex_t verrou; 
  pthread_cond_t busVide;    // pour attendre/avertir quand le bus est
			     // vide et réagir en conséquence
  pthread_cond_t busPlein;   // pour attendre/avertir quand le bus est
			     // plein et réagir en conséquence
  pthread_cond_t arriveeDestination;   // pour gérer l'arrivée à
				       // destination et déclancher la
				       // descente des passagers
};

struct params {    // structure pour regrouper les paramètres d'un thread. 
  
  int idThread;    // un identifiant de thread, 0 pour le bus et de 1 à N
		   // pour les passagers (N le nombre total de passagers)
  struct etatBus * varPartagee;  
};

// le bus : il attend que que le bus soit plein avant de partir,
// effectue le voyage, averti les passagers de l'arrivée à
// destination, attend que tous les passagers soient descendus,
// autorise les passagers suivants à monter, et recommence.
void * bus (void * p){ 
  
  struct params * args = (struct params *) p;
  struct etatBus * predicat = args -> varPartagee;

  int res;

  while(1){

    // le bus doit attendre qu'il soit plein avant de pouvoir partir.
    if  (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilité aussi de faire exit(..)
    }
    
    while  (predicat->pl_occupees <  predicat->capacite) {    
      cout <<"Bus : j'ettends que le bus soit rempli"<<endl;
      if (pthread_cond_wait(&(predicat->busPlein), &(predicat->verrou)) != 0){
	perror("erreur wait"); 
	pthread_exit(NULL); // possibilité aussi de faire exit(..)
      }
    }

    predicat->etat = 2 ;           // plus personne ne monte ni descend. Cette
				   // instruction peut être executée
				   // par le passager qui monte en
				   // dernier.
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0) {
      perror("erreur unlock"); break; // possibilité aussi de faire exit(..)
    }
 
    // le bus est plein, il peut partir. Simulation d'un temps de
    // voyage. Pendant ce temps, les passagers attendent que le bus
    // arrive à destination. 

    cout <<"Bus : depart"<<endl;
    sleep (3 + rand() % 7);

    // A l'arrivee, le bus signale l'ouverture de la porte de sortie (passage a l'état descente.
    if (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilité aussi de faire exit(..)
    }
    predicat->etat = 1;
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0) {
      perror("erreur unlock"); break; // possibilité aussi de faire exit(..)
    }
    cout <<"Bus : je suis arrive a destination, tout le monde descend"<<endl;
    if (pthread_cond_broadcast(&(predicat->arriveeDestination)) != 0){
      perror("erreur broadcast"); break; // possibilité aussi de faire exit(..)
    }
    
    // le bus attend que tous les passagers descendent avant
    // d'autoriser à nouveau la montee
    if (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilité aussi de faire exit(..)
    }
    while  (predicat->pl_occupees != 0) {
      cout <<"Bus : j'ettends que tous les passagers soient descendus"<<endl;
      if (pthread_cond_wait(&(predicat->busVide), &(predicat->verrou)) != 0){
	perror("erreur wait"); 
	pthread_exit(NULL); // possibilité aussi de faire exit(..)
      }
    }
    
    cout <<"Bus : tout le monde est descendu"<<endl;
    // d'autres passsagers peuvent monter.
    predicat->etat = 0; 
    if (pthread_cond_broadcast(&(predicat->busVide)) != 0){
      perror("erreur broadcast"); break; // possibilité aussi de faire exit(..)
    }
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0){
      perror("erreur unlock"); break; // possibilité aussi de faire exit(..)
    }
  }

  pthread_exit(NULL); 
 }

// le passager: essaye de monter dans le bus, si le bus est plein, il
// attend le prochain voyage. Une fois dans le bus, il attend fait le
// voyage en attendant l'arrivée du bus à destination. Il descend et
// recommence.
void * passager (void * p){ 
  
  struct params * args = (struct params *) p;
  struct etatBus * predicat = args -> varPartagee;

  while(1){

    // une simulation d'un temps pour espacer la montee des passagers
    // et mieux observer le comportement des threads à l'exécution
    sleep (args->idThread + rand() % args->idThread);

    cout <<"Passager "<< args->idThread <<" : arrive"<<endl;

    // j'attends l'ouverture de la porte. Si une place est disponible,
    // je monte, sinon j'attend un futur depart.
    if (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilité aussi de faire exit(..)
    }
    // la deuxieme partie de cette condition permet d'éviter de monter
    // pendant la descente des passagers à bord
    while  (predicat->pl_occupees ==  predicat->capacite || predicat->etat != 0) {
      cout <<"Passager "<< args->idThread <<" : attend avant de pouvoir monter"<<endl;
      if (pthread_cond_wait(&(predicat->busVide), &(predicat->verrou)) != 0){
	perror("erreur wait"); 
	pthread_exit(NULL); // possibilité aussi de faire exit(..)
      }
    }

    // je monte
    cout <<"Passager "<< args->idThread <<" : monte"<<endl;
    predicat->pl_occupees++;
 
    // si je suis le dernier, le bus doit partir
    if (predicat->pl_occupees ==  predicat->capacite)
      if (pthread_cond_signal(&(predicat->busPlein)) != 0){ // signal suffit car seul le bus
	                                                     // attend sur cette variable
						             // conditionnelle (broadcast est
						             // aussi correct)
	perror("erreur signal"); break; // possibilité aussi de faire exit(..)
      } 
						  
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0){
      perror("erreur unlock"); break; // possibilité aussi de faire exit(..)
    }
 
    // attendre que le bus arrive à destination. Remarque : il y a
    // obligation de libérer le verrou avant de le reprendre, sinon le
    // bus ne peut pas partir. 
    if (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilité aussi de faire exit(..)
    }
    while (predicat->etat != 1)
      if(pthread_cond_wait(&(predicat->arriveeDestination), &(predicat->verrou)) != 0){
	perror("erreur wait"); break; // possibilité aussi de faire exit(..)
    }

    cout <<"Passager "<< args->idThread <<" : descend"<<endl;
    predicat->pl_occupees--; // je descends et si je suis le dernier,
			     // je réveille ceux qui sont en attente
			     // que le bus soit vide (bus et passagers).
    if (predicat->pl_occupees == 0)
      if(pthread_cond_broadcast(&(predicat->busVide)) != 0){
	perror("erreur broadcast"); break; // possibilité aussi de faire exit(..)
      }
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0){
      perror("erreur unlock"); break; // possibilité aussi de faire exit(..)
    }
  }

  pthread_exit(NULL); 
}


int main(int argc, char * argv[]){
  
  if (argc!=3) {
    cout << " argument requis " << endl;
    cout << "./prog capacite_bus nombre_passagers" << endl;
    exit(1);
  }

  if (atoi(argv[2]) < atoi(argv[1])){
    cout << " le nombre de passagers doit etre >= a la capacite du bus " << endl;
    exit(1);
  }
 
  // initialisations 
  pthread_t threads[1 + atoi(argv[2])];
  struct params tabParams[1 + atoi(argv[2])];
  struct etatBus varPartagee;

  varPartagee.capacite = atoi(argv[1]);
  varPartagee.pl_occupees = 0;
  varPartagee.etat = 0;
 
  if (pthread_mutex_init(&(varPartagee.verrou), NULL)  != 0){
    perror("erreur init"); 
    exit(1);
  }
  if (pthread_cond_init(&(varPartagee.busVide), NULL) != 0){
    perror("erreur init"); 
    exit(1);
  }
  if (pthread_cond_init(&(varPartagee.busPlein), NULL) != 0){
    perror("erreur init"); 
    exit(1);
  }
  if (pthread_cond_init(&(varPartagee.arriveeDestination), NULL) != 0){
    perror("erreur init"); 
    exit(1);
  }

  srand(atoi(argv[1]));  // initialisation de rand pour la simulation de longs calculs
 
  // création des threards 

  // le bus
  tabParams[0].idThread = 0;
  tabParams[0].varPartagee = &varPartagee; 
  if (pthread_create(&threads[0], NULL, bus, &(tabParams[0])) != 0){
    perror("erreur creation thread");
    exit(1);
  }

  // les passagers
  for (int i = 1; i <= atoi(argv[2]); i++){
    tabParams[i].idThread = i;
    tabParams[i].varPartagee = &varPartagee; 
    // Le passage de paramètre est fortement conseillé (éviter les
    // variables globles).
    if (pthread_create(&threads[i], NULL, passager, &(tabParams[i])) != 0){
      perror("erreur creation thread");
      exit(1);
    }
  }

  // attente de la fin des  threards. 
  for (int i = 0; i < atoi(argv[2]) + 1; i++)
    if (pthread_join(threads[i], NULL) != 0){
      perror("erreur attente thread");
      exit(1);
    }

  // terminaison "propore". Partie fortement recommandée. 
  pthread_mutex_destroy(&(varPartagee.verrou));
  pthread_cond_destroy(&(varPartagee.busVide));
  pthread_cond_destroy(&(varPartagee.busPlein));
  pthread_cond_destroy(&(varPartagee.arriveeDestination));
}
 
