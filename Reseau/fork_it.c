#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void largeur(int l) {
	int i=0, pid;
	for(i=0;i<l;i++) {
		switch(pid=fork()) {
			case -1:
				printf("Erreur\n");
				exit(1);
			case 0:
				sleep(1);
				exit(0);
			default:
				printf("Pere (%d) du fils (%d)\n", getpid(), pid);
				break;
		}
	}
	if(pid==0) { //le père attend tout ses fils
		wait(0);
	}
}

void fili(int p) {
	int i, pid;
	for(i=0;i<p;i++) {
		switch(pid=fork()) {
			case -1:
				printf("Erreur\n");
				exit(1);
			case 0:
				sleep(1);
			 	break;
			default:
				printf("Pere (%d) du fils (%d)\n", getpid(), pid);
				wait(0);
				exit(0);
		}
	}
}

void binaire(int p) {
	int niv = 0;
	while(niv<p) {
		if(fork() == 0) {
			printf("Fils gauche %d de %d de niv %d\n", getpid(), getppid(), niv);
			niv++;
		}
		else if(fork() == 0) {
			printf("Fils droit %d de %d de niv %d\n", getpid(), getppid(), niv);
			niv++;
		}
		else break;
	}
	wait(0);

}


int main(int argc, char** argv)
{
	if(argc!=3){
		printf("Erreur d'arguments: Lancer main x y avec x:le choix de l'arbre et y:la taille de l'arbre");
	}
	else if(atoi(argv[1])==1){
		largeur(atoi(argv[2]));
	}
	else if(atoi(argv[1])==2){
		fili(atoi(argv[2]));
	}
	else if(atoi(argv[1])==3){
		binaire(atoi(argv[2]));
	}
	return 0;
}
