#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define psleep(sec) sleep ((sec))
#define N 40 // nombre de places
#define NbP 45 // nombre de passagers total voulant prendre le bus



typedef struct {
  int nbPB; //nombre de passagers dans le bus, max 40, min 0 = bus vide

  pthread_t thBus;
  pthread_t thPassagers [NbP];

  pthread_mutex_t mutex_places;
  pthread_cond_t cond_busVide;
  pthread_cond_t cond_busPlein;
  pthread_cond_t cond_finVisite;
    pthread_cond_t cond_busPret;

} bus_t;



//Au début, le bus est vide
static bus_t bus = {
  .nbPB = 0,
  .mutex_places = PTHREAD_MUTEX_INITIALIZER,
  .cond_busVide = PTHREAD_COND_INITIALIZER,
  .cond_busPlein = PTHREAD_COND_INITIALIZER,
  .cond_finVisite = PTHREAD_COND_INITIALIZER,
  .cond_busPret = PTHREAD_COND_INITIALIZER,
};


/***********************
Fonction du thread bus
***********************/
static void * fn_bus (void * p_data)
{
   while (1)
     {

       pthread_mutex_lock (& bus.mutex_places);

       if (bus.nbPB < 40)// Bus non plein
	 {
	   pthread_cond_wait(& bus.cond_busPlein, & bus.mutex_places);


	 }
       //Apres le wait le bus est plein, il fait son tour
       printf ("Le bus a atteint le nombre de passagers maximum.\n Départ imminent !\n");
       sleep(4);
       printf("Fin de la visite.\n Descente des passagers...\n");
       sleep(1);
       	pthread_cond_broadcast(& bus.cond_finVisite);
        pthread_cond_wait(& bus.cond_busVide, & bus.mutex_places);

       // Bus non vide

       //Le bus est vide
       if( bus.nbPB == 0){
       printf("Bus vide. Les prochains passagers peuvent monter.\n");
       pthread_cond_broadcast (& bus.cond_busPret);
     }
    }
 pthread_mutex_unlock (& bus.mutex_places);
   return NULL;
}



/********************************
 Fonction des threads passagers
*******************************/
static void * fn_passager (void * p_data)
{
  int id = (int )(intptr_t)p_data;

   while (1)
   {
        pthread_mutex_lock (& bus.mutex_places);

          if (bus.nbPB < 40){ // bus vide
     	      //  pthread_cond_signal (& bus.cond_busVide);
            bus.nbPB ++;
     	      printf("  Un passager %d est monté.\n Nombre de passagers à bord : %d \n", id, bus.nbPB);

                    if (bus.nbPB ==40) //le bus est plein
                    {
            	          pthread_cond_signal (& bus.cond_busPlein);
                      }

                      pthread_cond_wait (& bus.cond_finVisite, & bus.mutex_places);
                      bus.nbPB --;
                      printf("Un passager  %d est descendu.\n Nombre de passagers à bord : %d\n ",id, bus.nbPB);


            	          //pthread_cond_wait (& bus.cond_finVisite, & bus.mutex_places);
     	  }


        if (bus.nbPB == 0){
          printf("Le bus est vide.\n  ");
          pthread_cond_broadcast (& bus.cond_busVide);
          pthread_cond_wait (& bus.cond_busPret, & bus.mutex_places);
        }

          //pthread_cond_wait (& bus.cond_busVide, & bus.mutex_places);
    /*  if (bus.nbPB == 0){
        	printf("Le bus est vide.\n  ");
        //pthread_cond_signal (& bus.cond_busVide);
        pthread_cond_wait (& bus.cond_busPret, & bus.mutex_places);
      }*/


pthread_mutex_unlock(& bus.mutex_places);


   }

   return NULL;
}

/***********************
 **********************/

int main (void)
{
   int i = 0;
   int ret = 0;


   /* Creation des threads. */
   printf ("Creation du thread bus !\n");
   ret = pthread_create (
      & bus.thBus, NULL,
      fn_bus, NULL
   );

   if (! ret)
   {
      printf ("Creation des threads passagers !\n");
      for (i = 0; i < NbP; i++)
      {
         ret = pthread_create (
            & bus.thPassagers [i], NULL,
            fn_passager, (void *) i);

         if (ret)
         {
            fprintf (stderr, "%s", strerror (ret));
         }
      }
   }
   else
   {
      fprintf (stderr, "%s", strerror (ret));
   }


   /* Attente de la fin des threads. */
   i = 0;
   for (i = 0; i < NbP; i++)
   {
      pthread_join (bus.thPassagers [i], NULL);
   }
   pthread_join (bus.thBus, NULL);


   return EXIT_SUCCESS;
}
