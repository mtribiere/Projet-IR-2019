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
  if(dogInfos->dogType == 1)
  {
    int sheepDetected = 0;
    int sheepToMove = 0;
    int sheep;
    // Détection des éventuelles brebis et sélection de celle à déplacer
    for (sheep=0; sheep < numberOfEntity; sheep++)
    {
      if(entityAround[sheep].nickname[0] == 'b' && entityAround[sheep].nickname[1] == 'o' entityAround[sheep].nickname[2] == 't')
      {
        sheepDetected = 1;
        sheepToMove = sheep;
      }
    }

    if(sheepDetected){
  	// Si la position est indéfinie, on lui fait rejoindre l'une des 4 positions définies
  	// Selon que la brebis est dans la partie supérieure ou inférieure de la carte, state change
  	if(dogInfos->state==0)
  	{
      printf("Le chien est dans l'état %d \n",dogInfos->state);
  		if(dogInfos->targetPositionX <= entityAround[sheepToMove].positionX && dogInfos->targetPositionY <= entityAround[sheepToMove].positionY)
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX)-dogInfos->actionRange;
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)-dogInfos->actionRange;
        if(isTargetPositionReached(dogInfos))
        {
  			     if(entityAround[sheepToMove].positionY < MAP_SIZE_Y) dogInfos->state=3;
  			     else dogInfos->state=1;
        }
  		}
  		else if(dogInfos->targetPositionX > entityAround[sheepToMove].positionX && dogInfos->targetPositionY <= entityAround[sheepToMove].positionY)
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX)+dogInfos->actionRange;
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)-dogInfos->actionRange;
        if(isTargetPositionReached(dogInfos))
        {
  			   if (entityAround[sheepToMove].positionY < MAP_SIZE_Y) dogInfos->state=4;
  			   else dogInfos->state=2;
        }
  		}
  		else if(dogInfos->targetPositionX <= entityAround[sheepToMove].positionX && dogInfos->targetPositionY > entityAround[sheepToMove].positionY)
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX)-dogInfos->actionRange;
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)+dogInfos->actionRange;
        if(isTargetPositionReached(dogInfos))
        {
  			   if(entityAround[sheepToMove].positionY < MAP_SIZE_Y) dogInfos->state=1;
  			   else dogInfos->state=3;
        }
  		}
  		else if(dogInfos->targetPositionX > entityAround[sheepToMove].positionX && dogInfos->targetPositionY > entityAround[sheepToMove].positionY)
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX)+dogInfos->actionRange;
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)+dogInfos->actionRange;
        if(isTargetPositionReached(dogInfos))
        {
  			     if(entityAround[sheepToMove].positionY < MAP_SIZE_Y) dogInfos->state=2;
  			     else dogInfos->state=4;
        }
  		}
  	}
  	// S'il est à la position 1, on l'emmène à la position 3 (qui change selon position brebis)
  	else if(dogInfos->state==1)
  	{
      printf("Le chien est dans l'état %d \n",dogInfos->state);
  		if(entityAround[sheepToMove].positionY < MAP_SIZE_Y)
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX)-dogInfos->actionRange;
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)-dogInfos->actionRange;
  		}
  		else
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX)-dogInfos->actionRange;
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)+dogInfos->actionRange;
  		}
      if(isTargetPositionReached(dogInfos))
      {
  		    dogInfos->state=3;
      }
  	}
  	// S'il est à la position 2, on l'emmène à la position 4 (qui change selon position brebis)
  	else if(dogInfos->state==2)
  	{
      printf("Le chien est dans l'état %d \n",dogInfos->state);
  		if(entityAround[sheepToMove].positionY < MAP_SIZE_Y)
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX)+dogInfos->actionRange;
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)-dogInfos->actionRange;
  		}
  		else
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX)+dogInfos->actionRange;
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)+dogInfos->actionRange;
  		}
      if(isTargetPositionReached(dogInfos))
      {
  		dogInfos->state=4;
      }
  	// S'il est à la position 3 ou à la position 4, on l'emmène à la position finale (qui change selon position brebis)
  	}
  	else if(dogInfos->state==3 || dogInfos->state==4)
  	{
      printf("Le chien est dans l'état %d \n",dogInfos->state);
  		if(entityAround[sheepToMove].positionY < MAP_SIZE_Y)
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX);
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)-dogInfos->actionRange;
  		}
  		else
  		{
  			dogInfos->targetPositionX=(entityAround[sheepToMove].positionX);
  			dogInfos->targetPositionY=(entityAround[sheepToMove].positionY)+dogInfos->actionRange;
  		}
      if(isTargetPositionReached(dogInfos))
      {
  		dogInfos->state=5;
      }
  	}
  	else if(dogInfos->state==5)
  	{
      printf("Le chien est dans l'état %d \n",dogInfos->state);
  		// On veut descendre verticalement => pas de changement pour positionX
      if(entityAround[sheepToMove].positionY < MAP_SIZE_Y) dogInfos->targetPositionY=(MAP_SIZE_Y/2)-dogInfos->actionRange;
  		else dogInfos->targetPositionY=(MAP_SIZE_Y/2)+dogInfos->actionRange;
  		dogInfos->state=6;
  	}
    else if(dogInfos->state==6)
  	{
      printf("Le chien est dans l'état %d \n",dogInfos->state);
      dogInfos->state = 0;
      if(isTargetPositionReached(dogInfos))
      {
        dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
        dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
      }
    }
  }
  else // Si aucune brebis n'a été détectée dans le champ de vision
  {
    if(isTargetPositionReached(dogInfos)){
      dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
      dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
    }
  }

  }
}
