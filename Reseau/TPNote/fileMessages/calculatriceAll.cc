#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
using namespace std;

//g++ calculatriceAll.cc -o exec 
/*
Supprimer toutes les files de message du système
ipcrm --all=msg
ipcrm -q cleCalc
*/

int  main (){

  //creation de la file de message -> CLEF
  key_t cle = ftok("cleCalc.txt", 'z');
  if (cle==-1) {
    perror("erreur ftok");
    exit(1);
  }
  cout << "ftok ok" << endl;


  int msgid = msgget(cle, IPC_CREAT|IPC_EXCL|0666); //Création a proprement parler de la file de message
  sleep(1);
  /*if ( msgctl(msgid, IPC_RMID, NULL) == -1 ) {
    perror("msgctl(IPC_RMID)");
    abort();
  }*/
  if(msgid == -1) {
    perror("msgget : file existante a supprimer ou autre erreur");

    exit(1);
  }
  cout << "msgget ok" << endl;

  struct req{
    long etiqReq; // doit etre a 1
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

  // reception et traitement des requetes
  while(1){

    if(msgrcv(msgid, (void *)&requete, sizeof(requete.contenu), 1, 0) == -1){
      perror("erreur rcv");
      exit(1);
    }

    cout <<"contenu du message lu :"<<endl;
    cout <<"etiquete :" <<requete.etiqReq<< ", op1 :"<<requete.contenu.op1<< ", op2 :"<<requete.contenu.op2<<endl;

    switch (requete.contenu.idop) {

    case 1 :
      reponse.valeur = requete.contenu.op1 + requete.contenu.op2;
      break;

    case 2 :
      reponse.valeur = requete.contenu.op1 - requete.contenu.op2;
      break;

    case 3 :
      reponse.valeur = requete.contenu.op1 * requete.contenu.op2;
      break;

    default:
      reponse.valeur = requete.contenu.op1 / requete.contenu.op2;
      break;

    }

    reponse.etiqRep = 2;
    // retour resultat
    if(msgsnd(msgid, (void *)&reponse, sizeof(reponse.valeur), 0)==-1){
      perror("erreur send");
      exit(1);
    }

  }
  //msgctl(msgid, IPC_RMID, NULL);

  return 0;
}
