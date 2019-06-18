#ifndef BRAIN
#define BRAIN

#include <sys/time.h>

#include "entity.h"

#define TIME_UNTIL_SYNCH 15
#define BASE_SIDE 1
/********
\ 1 : Base a gauche
\ 2 : Base a droite
********/

void computeStrategy(Dog *dogInfos,Entity *entityAround,int numberOfEntity);
int findIdOfSheep(Entity *entityAround,int numberOfEntity,int idToFind);
int positionClamp(int targetPosition,int type);
int isInBase(int positionX,int positionY);
int isPushingPositionReached(int positionX,int positionY,int targetPositionX,int targetPositionY);
int isTargetPositionReached(Dog *dogInfos);
int generateRandomPosition(int lower,int upper);

//Initialisation des variables de backup au cas où la brebis sort du champ de vision du chien (dans le mode duel)
int Target_X_Backup;
int Target_Y_Backup;

#endif
