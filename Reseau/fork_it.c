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

int binaire(int p) {
	int niv = 0;
	while(niv<p) {
		if(fork() == 0) {
			printf("Fils gauche %d de %d de niv %d\n", getpid(), getppid(), niv);
			niv++;
			sleep(5);
		}
		else if(fork() == 0) {
			printf("Fils droit %d de %d de niv %d\n", getpid(), getppid(), niv);
			niv++;
			sleep(5);
		}
		else {
				while (wait(0)!=-1) {}
				break;
		}
	}
	return 0;

}


int main(int argc, char** argv)
{
	if(argc == 2 && strcmp(argv[1], "help") == 0 )
	{
		printf("\nLancer avec : fork choixArbre taille\n\nchoixArbre:\n largeur\n filiforme\n binaire\n\ntaille: int > 0 \n\n\n ");
		return 0;
	}
	else if(argc!=3){
		printf("\nErreur d'arguments!\n\n Lancer avec : fork choixArbre taille.\n Pour plus de détails tapez fork help.\n\n\n ");
		return 0;
	}

	if(strcmp(argv[1], "largeur") == 0){
		largeur(atoi(argv[2]));
	}
	else if(strcmp(argv[1], "filiforme") == 0){
		fili(atoi(argv[2]));
	}
	else if(strcmp(argv[1], "binaire") ==0 ){
		binaire(atoi(argv[2]));
	}
	else {
		printf("Coucou je suis la %s\n", argv[1] );
	}
	return 0;
}
