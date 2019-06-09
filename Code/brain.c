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

int findIdOfSheep(Entity *entityAround,int numberOfEntity,int idToFind){

  int idToReturn = 0;
  for(int i = 0;i<numberOfEntity;i++){
    if(entityAround[i].ID == idToFind) idToReturn = i;
  }

  return idToReturn;
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

  //Si on est un chien Vert (Ramener les brebis sur la ligne centrale)
  if(dogInfos->dogType == 1)
  {

    //Si on est en recherche de brebis
    if(dogInfos->state == 0){

      int tmpIdSheep;
      //Chercher si une brebis est visible
      for(int i = 0;i<numberOfEntity;i++){

        //Si c'est une brebis
        if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){
          //Si elle n'est pas déjà placé
          if(!(entityAround[i].positionY>= (MAP_SIZE_Y/2)-PUSHING_MARGIN && entityAround[i].positionY <= (MAP_SIZE_Y/2)+PUSHING_MARGIN)){
            //La cibler
            dogInfos->targetedSheepId = entityAround[i].ID;
            tmpIdSheep = i;

            printf("Y Sheep : %d\n",entityAround[i].positionY);
          }
        }
      }

      //Si une brebis est proche
      if(dogInfos->targetedSheepId != 0){
        //Si elle est au dessus de nous
        if(entityAround[tmpIdSheep].positionY <= (dogInfos->entity).positionY){

          //Si on est dans la partie haute de la map
          if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){

            dogInfos->targetPositionX = (dogInfos->entity).positionX;
            dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY-(dogInfos->actionRange);

          }else{//Si on est dans la partie basse

            dogInfos->targetPositionX = (dogInfos->entity).positionX;
            dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY+(dogInfos->actionRange);
          }

        }else{//Si elle est au dessous de nous

          //Si on est dans la partie haute de la map
          if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){

            dogInfos->targetPositionX = (dogInfos->entity).positionX;
            dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY-(dogInfos->actionRange);

          }else{//Si on est dans la partie basse

            dogInfos->targetPositionX = (dogInfos->entity).positionX;
            dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY+(dogInfos->actionRange);

          }
        }
        dogInfos->state = 2;
      }else{ //Si aucune brebis visible
        //Si on est arrivé a destination
        if(isTargetPositionReached(dogInfos)){
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
      }
    }else{//Si on est en chasse

      //Si on est pas encore aligné et qu'on a fini la manoeuvre précédente
      if(dogInfos->state == 2 && isTargetPositionReached(dogInfos)){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);


          dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX;
          dogInfos->targetPositionY = (dogInfos->entity).positionY;

         dogInfos->state=3;

      }

      //Si on est alligné
      if(dogInfos->state == 3 && isTargetPositionReached(dogInfos))
        dogInfos->state = 4; // On passe a l'état de déplacement de brebis

      //Etat de déplacement de brebis
      if(dogInfos->state == 4){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

        //Si la brebis n'est pas au milieu
        if(!(entityAround[tmpIdSheep].positionY >= MAP_SIZE_Y/2-PUSHING_MARGIN && entityAround[tmpIdSheep].positionY <= MAP_SIZE_Y/2+PUSHING_MARGIN)){

            //Continuer de pousser la brebis
            dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX;

            //Si on est dans le haut de la map
            if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
              dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY-(dogInfos->actionRange)+2;
            }else{
              dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY+(dogInfos->actionRange)-2;
            }

        }else{ // Brebis en position

          //Arreter la chasse
          dogInfos->state = 0;
          dogInfos->targetedSheepId = 0;

          //Partir ailleurs
          printf("Fin de la chasse\n");
        }
      }

    }
  }
}
