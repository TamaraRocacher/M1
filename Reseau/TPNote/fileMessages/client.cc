#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

int main (int argc, char * argv[]){

  if(argc != 3){
    cerr << "lancement : ./client operande1 operande2"<< endl;
    exit(1);
  }

  // récuperer l'identifiant de la file de message.
  key_t cle = ftok("cleCalc.txt", 'z');
  if (cle ==-1) {
    perror("erreur ftok");
    exit(1);
  }
  cout << "ftok ok" << endl;
  int msgid = msgget(cle, 0666);
  if(msgid == -1) {
    perror("erreur msgget");
    exit(1);
  }
  cout << "msgget ok" << endl;

  // structure des requetes et réponses. Les étiquetes des deux
  // types de messages doivent être différentes, d'une par parce
  // qu'ils n'ont pas le même contenu et d'une autre part pour
  // éviter la situation de lecture d'un message de n'importe quelle
  // etiquette. Ce dernier cas peut aboutir à la lecture d'un
  // message qui nous est pas destiné, exemple: le client lit sa propre
  // requete en pensant que c'est une réponse.
  struct req{
    long etiqReq; // 1
    struct contenu{
      int idop;  // 1: +, 2 : -, 3: *, 4: /
      int op1;
      int op2;
    } contenu;
  } requete;

  struct reponse{
    long etiqRep; // 2
    int valeur;
  }reponse;   

  
  // test des 4 opérations :

  // ce qui ne change pas pour l'ensemble des requetes.
  requete.etiqReq = (long) 1;
  requete.contenu.op1 = atoi(argv[1]);
  requete.contenu.op2 = atoi(argv[2]);
  
  for(int i = 1; i < 5 ; i++){
    
    cout <<"Test : "<< i <<endl;
    requete.contenu.idop = i;
  
    // envoi requete 
    if(msgsnd(msgid, (void *)&requete, sizeof(requete.contenu), 0) ==-1){
      perror("erreur send");
      exit(1);
    }

    // reception résultat
    if(msgrcv(msgid, (void *)&reponse, sizeof(reponse.valeur), 2, 0)==-1){ // 2 -> etiquette (onglet) de reception du message
      perror("erreur rcv");
      exit(1);
    }
    cout <<"Contenu du message recu : "<<endl;
    cout <<"etiquete :" <<reponse.etiqRep<< " resultat :"<<reponse.valeur<<endl;
  }
  return 0;
}
