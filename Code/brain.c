#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include "entity.h"
#include "map.h"


int generateRandomPosition(int lower,int upper){
  srandom(time(NULL));
  return (rand() % (upper - lower + 1)) + lower;
}

int isTargetPositionReached(Dog *dogInfos){

  int toReturn = 0;

  //Si on a atteint la destination en X
  if((dogInfos->entity).positionX <= (dogInfos->targetPositionX)+POSITION_MARGIN && (dogInfos->entity).positionX >= (dogInfos->targetPositionX)-POSITION_MARGIN){
    //Si on atteint la destination en Y
    if((dogInfos->entity).positionY <= (dogInfos->targetPositionY)+POSITION_MARGIN && (dogInfos->entity).positionY >= (dogInfos->targetPositionY)-POSITION_MARGIN){
      toReturn = 1;
    }
  }

  return toReturn;
}


/**************************
\Calcule la strategie (Déplacements aléatoires + ramener brebis)
***************************/
/*
 void computeStrategy(Dog *dogInfos,Entity *entityAround,int numberOfEntity){

  if(isTargetPositionReached(dogInfos)){
    dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
    dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
  }
}
*/


/***************************
\Strategie pour les chiens Verts et Jaune
****************************/
void computeStrategy(Dog *dogInfos, Entity *entityAround, int numberOfEntity)
{
  //Si on est un chien Jaune (Simple carré)
  if(dogInfos->dogType == 5){

    //Verifier si une position est atteinte
    if(isTargetPositionReached(dogInfos)){
      (dogInfos->state)++;

      //Boucler
      if(dogInfos->state >= 4) dogInfos->state = 0;
    }

    //Etat initial / Point 0
    if(dogInfos->state == 0){
      dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
      dogInfos->targetPositionY = MAP_SIZE_Y/2;
    }

    //Point 1
    if(dogInfos->state == 1){
      dogInfos->targetPositionX = ENTITY_SIZE;
      dogInfos->targetPositionY = MAP_SIZE_Y/2;
    }

    //Point 2
    if(dogInfos->state == 2){
      dogInfos->targetPositionX = ENTITY_SIZE;
      dogInfos->targetPositionY = MAP_SIZE_Y-ENTITY_SIZE;
    }

    //Point 3
    if(dogInfos->state == 3){
      dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
      dogInfos->targetPositionY = MAP_SIZE_Y-ENTITY_SIZE;
    }

  }

  //Si on est un chien Vert
  if(dogInfos->dogType == 1){

  }

}
