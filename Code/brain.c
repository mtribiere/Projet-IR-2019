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
\Strategie pour les chiens verts
****************************/

// ATTENTION CONTOURNEMENT DIFFERENT SELON SI BREBIS DANS PARTIE INFERIEURE OU SUPERIEURE DE LA CARTE!!!
// 0 : Position indéfinie
// 1 : Rayon plus bas que brebis, Rayon plus sur la gauche
// 2 : Rayon plus bas que brebis, Rayon plus sur la droite
// 3 : Rayon plus haut que brebis, Rayon plus sur la gauche
// 4 : Rayon plus haut que brebis, Rayon plus sur la droite
// 5 : Position finale atteinte, on est prêt à pousser brebis vers destination
// 6 : Position où la brebis a été amenée à destination
// On considère que la hauteur de la brebis est différente de Largeur / 2 (sinon pas de déplacement à faire)

void computeStrategy(Dog *dogInfos, Entity *entityAround, int numberOfEntity)
// Rajouter les [numberOfEntity]
{

  int sheepDetected = 0;

  if(sheepDetected){
	// Si la position est indéfinie, on lui fait rejoindre l'une des 4 positions définies
	// Selon que la brebis est dans la partie supérieure ou inférieure de la carte, state change
	if(dogInfos->state==0)
	{
		if(dogInfos->targetPositionX <= entityAround[numberOfEntity].positionX && dogInfos->targetPositionY <= entityAround[numberOfEntity].positionY)
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX)-dogInfos->actionRange;
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)-dogInfos->actionRange;
			if(entityAround[numberOfEntity].positionY < MAP_SIZE_Y) dogInfos->state=3;
			else dogInfos->state=1;
		}
		else if(dogInfos->targetPositionX > entityAround[numberOfEntity].positionX && dogInfos->targetPositionY <= entityAround[numberOfEntity].positionY)
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX)+dogInfos->actionRange;
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)-dogInfos->actionRange;
			if (entityAround[numberOfEntity].positionY < MAP_SIZE_Y) dogInfos->state=4;
			else dogInfos->state=2;
		}
		else if(dogInfos->targetPositionX <= entityAround[numberOfEntity].positionX && dogInfos->targetPositionY > entityAround[numberOfEntity].positionY)
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX)-dogInfos->actionRange;
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)+dogInfos->actionRange;
			if(entityAround[numberOfEntity].positionY < MAP_SIZE_Y) dogInfos->state=1;
			else dogInfos->state=3;
		}
		else if(dogInfos->targetPositionX > entityAround[numberOfEntity].positionX && dogInfos->targetPositionY > entityAround[numberOfEntity].positionY)
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX)+dogInfos->actionRange;
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)+dogInfos->actionRange;
			if(entityAround[numberOfEntity].positionY < MAP_SIZE_Y) dogInfos->state=2;
			else dogInfos->state=4;
		}
	}
	// S'il est à la position 1, on l'emmène à la position 3 (qui change selon position brebis)
	else if(dogInfos->state==1)
	{
		if(entityAround[numberOfEntity].positionY < MAP_SIZE_Y)
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX)-dogInfos->actionRange;
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)-dogInfos->actionRange;
		}
		else
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX)-dogInfos->actionRange;
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)+dogInfos->actionRange;
		}
		dogInfos->state=3;
	}
	// S'il est à la position 2, on l'emmène à la position 4 (qui change selon position brebis)
	else if(dogInfos->state==2)
	{
		if(entityAround[numberOfEntity].positionY < MAP_SIZE_Y)
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX)+dogInfos->actionRange;
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)-dogInfos->actionRange;
		}
		else
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX)+dogInfos->actionRange;
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)+dogInfos->actionRange;
		}
		dogInfos->state=4;
	// S'il est à la position 3 ou à la position 4, on l'emmène à la position finale (qui change selon position brebis)
	}
	else if(dogInfos->state==3 || dogInfos->state==4)
	{
		if(entityAround[numberOfEntity].positionY < MAP_SIZE_Y)
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX);
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)-dogInfos->actionRange;
		}
		else
		{
			dogInfos->targetPositionX=(entityAround[numberOfEntity].positionX);
			dogInfos->targetPositionY=(entityAround[numberOfEntity].positionY)+dogInfos->actionRange;
		}
		dogInfos->state=5;
	}
	else if(dogInfos->state==5)
	{
		// On veut descendre verticalement => pas de changement pour positionX
		dogInfos->targetPositionY=(MAP_SIZE_Y/2)+dogInfos->actionRange;
		dogInfos->state=6;
	}

  }
}
