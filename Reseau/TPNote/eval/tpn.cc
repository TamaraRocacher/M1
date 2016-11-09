#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// variables partag�es g�rant l'�tat du bus
struct etatBus {

  int pl_occupees;           // pour compter le nombre de passagers dans le bus
  int capacite;              // le nombre max de passagers accept�s dans le bus
  int etat;                  // variable initialis�e � 0 pour
			     // d�clancher la mont�e, mise � 1 pour
			     // d�clancher la descente (fin du
			     // voyage), mise � 2, pendant le voyage
			     // (descente et mont�e non autoris�e)
  pthread_mutex_t verrou; 
  pthread_cond_t busVide;    // pour attendre/avertir quand le bus est
			     // vide et r�agir en cons�quence
  pthread_cond_t busPlein;   // pour attendre/avertir quand le bus est
			     // plein et r�agir en cons�quence
  pthread_cond_t arriveeDestination;   // pour g�rer l'arriv�e �
				       // destination et d�clancher la
				       // descente des passagers
};

struct params {    // structure pour regrouper les param�tres d'un thread. 
  
  int idThread;    // un identifiant de thread, 0 pour le bus et de 1 � N
		   // pour les passagers (N le nombre total de passagers)
  struct etatBus * varPartagee;  
};

// le bus : il attend que que le bus soit plein avant de partir,
// effectue le voyage, averti les passagers de l'arriv�e �
// destination, attend que tous les passagers soient descendus,
// autorise les passagers suivants � monter, et recommence.
void * bus (void * p){ 
  
  struct params * args = (struct params *) p;
  struct etatBus * predicat = args -> varPartagee;

  int res;

  while(1){

    // le bus doit attendre qu'il soit plein avant de pouvoir partir.
    if  (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilit� aussi de faire exit(..)
    }
    
    while  (predicat->pl_occupees <  predicat->capacite) {    
      cout <<"Bus : j'ettends que le bus soit rempli"<<endl;
      if (pthread_cond_wait(&(predicat->busPlein), &(predicat->verrou)) != 0){
	perror("erreur wait"); 
	pthread_exit(NULL); // possibilit� aussi de faire exit(..)
      }
    }

    predicat->etat = 2 ;           // plus personne ne monte ni descend. Cette
				   // instruction peut �tre execut�e
				   // par le passager qui monte en
				   // dernier.
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0) {
      perror("erreur unlock"); break; // possibilit� aussi de faire exit(..)
    }
 
    // le bus est plein, il peut partir. Simulation d'un temps de
    // voyage. Pendant ce temps, les passagers attendent que le bus
    // arrive � destination. 

    cout <<"Bus : depart"<<endl;
    sleep (3 + rand() % 7);

    // A l'arrivee, le bus signale l'ouverture de la porte de sortie (passage a l'�tat descente.
    if (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilit� aussi de faire exit(..)
    }
    predicat->etat = 1;
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0) {
      perror("erreur unlock"); break; // possibilit� aussi de faire exit(..)
    }
    cout <<"Bus : je suis arrive a destination, tout le monde descend"<<endl;
    if (pthread_cond_broadcast(&(predicat->arriveeDestination)) != 0){
      perror("erreur broadcast"); break; // possibilit� aussi de faire exit(..)
    }
    
    // le bus attend que tous les passagers descendent avant
    // d'autoriser � nouveau la montee
    if (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilit� aussi de faire exit(..)
    }
    while  (predicat->pl_occupees != 0) {
      cout <<"Bus : j'ettends que tous les passagers soient descendus"<<endl;
      if (pthread_cond_wait(&(predicat->busVide), &(predicat->verrou)) != 0){
	perror("erreur wait"); 
	pthread_exit(NULL); // possibilit� aussi de faire exit(..)
      }
    }
    
    cout <<"Bus : tout le monde est descendu"<<endl;
    // d'autres passsagers peuvent monter.
    predicat->etat = 0; 
    if (pthread_cond_broadcast(&(predicat->busVide)) != 0){
      perror("erreur broadcast"); break; // possibilit� aussi de faire exit(..)
    }
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0){
      perror("erreur unlock"); break; // possibilit� aussi de faire exit(..)
    }
  }

  pthread_exit(NULL); 
 }

// le passager: essaye de monter dans le bus, si le bus est plein, il
// attend le prochain voyage. Une fois dans le bus, il attend fait le
// voyage en attendant l'arriv�e du bus � destination. Il descend et
// recommence.
void * passager (void * p){ 
  
  struct params * args = (struct params *) p;
  struct etatBus * predicat = args -> varPartagee;

  while(1){

    // une simulation d'un temps pour espacer la montee des passagers
    // et mieux observer le comportement des threads � l'ex�cution
    sleep (args->idThread + rand() % args->idThread);

    cout <<"Passager "<< args->idThread <<" : arrive"<<endl;

    // j'attends l'ouverture de la porte. Si une place est disponible,
    // je monte, sinon j'attend un futur depart.
    if (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilit� aussi de faire exit(..)
    }
    // la deuxieme partie de cette condition permet d'�viter de monter
    // pendant la descente des passagers � bord
    while  (predicat->pl_occupees ==  predicat->capacite || predicat->etat != 0) {
      cout <<"Passager "<< args->idThread <<" : attend avant de pouvoir monter"<<endl;
      if (pthread_cond_wait(&(predicat->busVide), &(predicat->verrou)) != 0){
	perror("erreur wait"); 
	pthread_exit(NULL); // possibilit� aussi de faire exit(..)
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
	perror("erreur signal"); break; // possibilit� aussi de faire exit(..)
      } 
						  
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0){
      perror("erreur unlock"); break; // possibilit� aussi de faire exit(..)
    }
 
    // attendre que le bus arrive � destination. Remarque : il y a
    // obligation de lib�rer le verrou avant de le reprendre, sinon le
    // bus ne peut pas partir. 
    if (pthread_mutex_lock(&(predicat->verrou)) != 0){
      perror("erreur lock"); break; // possibilit� aussi de faire exit(..)
    }
    while (predicat->etat != 1)
      if(pthread_cond_wait(&(predicat->arriveeDestination), &(predicat->verrou)) != 0){
	perror("erreur wait"); break; // possibilit� aussi de faire exit(..)
    }

    cout <<"Passager "<< args->idThread <<" : descend"<<endl;
    predicat->pl_occupees--; // je descends et si je suis le dernier,
			     // je r�veille ceux qui sont en attente
			     // que le bus soit vide (bus et passagers).
    if (predicat->pl_occupees == 0)
      if(pthread_cond_broadcast(&(predicat->busVide)) != 0){
	perror("erreur broadcast"); break; // possibilit� aussi de faire exit(..)
      }
    if (pthread_mutex_unlock(&(predicat->verrou)) != 0){
      perror("erreur unlock"); break; // possibilit� aussi de faire exit(..)
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
 
  // cr�ation des threards 

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
    // Le passage de param�tre est fortement conseill� (�viter les
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

  // terminaison "propore". Partie fortement recommand�e. 
  pthread_mutex_destroy(&(varPartagee.verrou));
  pthread_cond_destroy(&(varPartagee.busVide));
  pthread_cond_destroy(&(varPartagee.busPlein));
  pthread_cond_destroy(&(varPartagee.arriveeDestination));
}
 
