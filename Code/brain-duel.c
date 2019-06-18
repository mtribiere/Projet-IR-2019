#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "entity.h"
#include "brain.h"
#include "map.h"

/***************************
\Stratégie d'extraction des brebis de l'enclos en mode duel pour Red et Cyan
****************************/
void computeStrategyDuel(Dog *dogInfos, Entity *entityAround, int numberOfEntity)
{
  if ((dogInfos->dogType == 2) || (dogInfos->dogType == 3)) {

    //Infiltration vers l'enclos adverse
    if(dogInfos->state == 0){

      //Aller au meetPoint
      dogInfos->targetPositionX = MAP_SIZE_X/28;
      dogInfos->targetPositionY = MAP_SIZE_Y/2;

      if (isTargetPositionReached(dogInfos)){
        dogInfos->state = 1;
        printf("Dog state = 1\n");
      }
    }

    //Detection des sheeps
    if(dogInfos->state == 1){

      int tmpIdSheep;
      //Chercher si une brebis est visible
      for(int i = 0; i < numberOfEntity; i++){

        //Si c'est une brebis
        if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){

          //Si elle est toujours dans l'enclos
          if(isInBase(entityAround[i].positionX,entityAround[i].positionY)){
            //La cibler
            dogInfos->targetedSheepId = entityAround[i].ID;
            tmpIdSheep = i;

            printf("Y Sheep : %d\n",entityAround[i].positionY);
            printf("dogInfos->targetedSheepId = %d\n\n",dogInfos->targetedSheepId);
          }
        }
      }

      if(dogInfos->targetedSheepId != 0){

        //Se positionnner de telle sorte qu'on puisse faire sortir la brebis par le haut
        dogInfos->targetPositionX = (dogInfos->entity).positionX;
        dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY-(dogInfos->actionRange);
        dogInfos->state = 2;

        printf("dogInfos->targetPositionX = %d ; dogInfos->targetPositionY = %d\n\n",dogInfos->targetPositionX,dogInfos->targetPositionY);
      }
    }

    //Si on est pas encore aligné et qu'on a fini la manoeuvre précédente
      if(dogInfos->state == 2 && isTargetPositionReached(dogInfos)){

        printf("Are you even there ??\n" );
        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);


          dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX;
          dogInfos->targetPositionY = (dogInfos->entity).positionY-(dogInfos->actionRange)/2;
          dogInfos->state = 3;
    }


    //Si on est aligné
    if(dogInfos->state == 3)
      dogInfos->state = 4; // On passe a l'état de déplacement de brebis

    //Etat de déplacement de brebis
    if(dogInfos->state == 4){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

        printf("(dogInfos State = 4) tmpIdSheep = %d\n",tmpIdSheep);
        if((tmpIdSheep > 0) && isInBase(entityAround[tmpIdSheep].positionX,entityAround[tmpIdSheep].positionY)){
          printf("cc\n");
          //Continuer de pousser la brebis
          dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX;
          dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY;

        }
      }
        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

        int Sheep_X = entityAround[tmpIdSheep].positionX;
        int Sheep_Y = entityAround[tmpIdSheep].positionY;
        int Dog_X = (dogInfos->entity).positionX;
        int Dog_Y = (dogInfos->entity).positionY;
        int Dog_TX = dogInfos->targetPositionX;
        int Dog_TY = dogInfos->targetPositionY;

        printf("Sheep_X = %d ; Sheep_Y = %d\n",Sheep_X,Sheep_Y);

        if((!(isInBase(Sheep_X,Sheep_Y)) && (dogInfos->targetedSheepId != 0) && (tmpIdSheep != -1)) || ((isPushingPositionReached(Dog_X,Dog_Y,Dog_TX,Dog_TY) && (dogInfos->state == 4)))){
         // Brebis en position

        // Arreter la chasse
        dogInfos->state = 0;
        dogInfos->targetedSheepId = 0;

        //Partir ailleurs
        printf("Fin de la chasse\n");
      }
    }
  }
