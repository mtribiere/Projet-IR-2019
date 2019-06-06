#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>

#include "entity.h"
#include "map.h"

/**************************
\Calcule la strategie (Déplacements aléatoires)
***************************/
void computeStrategy(Dog *dogInfos,Entity *entityAround,int numberOfEntity){

  //Si on a atteint la destination
  if((dogInfos->entity).positionX == dogInfos->targetPositionX && (dogInfos->entity).positionY == dogInfos->targetPositionY){

    srandom(time(NULL));

    dogInfos->targetPositionX = (rand()%(MAP_SIZE_X-2*ENTITY_SIZE)) + ENTITY_SIZE;
    dogInfos->targetPositionY = (rand()%(MAP_SIZE_Y-2*ENTITY_SIZE)) + ENTITY_SIZE;
  }

}
