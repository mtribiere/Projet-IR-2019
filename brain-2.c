#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "entity.h"
#include "brain.h"
#include "brain-duel.h"
#include "map.h"

#define TIME_UNTIL_SYNCH 15


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


int isYellowIn(Dog *dogInfos,Entity *entityAround,int numberOfEntity){

  int i;
  int toReturn = 0;

  for (i=0;i < numberOfEntity;i++){
    // Si l'entité est un jaune
    if(dogInfos->dogType == 5){
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


int min(int a, int b){
  int toReturn=a;
  if(b < a){
    toReturn = b;
  }
  return toReturn;
}


int isEntityInBase(Entity entity){
  int toReturn = 0;
  unsigned int distance = sqrt(pow(entity.positionX,2)+pow(abs(MAP_SIZE_Y/2 - entity.positionY),2));
  if(distance < MAP_SIZE_X/10) toReturn = 1;

  return toReturn;
}


int positionClamp(int targetPosition,int type){
    int positionToReturn = targetPosition;

    //Si c'est une position X
    if(type == 0){

        //Si inférieur à la limite
        if(targetPosition < ENTITY_SIZE) positionToReturn = ENTITY_SIZE;
        //Si supérieur à la limite
        if(targetPosition > MAP_SIZE_X - ENTITY_SIZE) positionToReturn = MAP_SIZE_X-ENTITY_SIZE;

    }

    //Si c'est un positionY
    if(type == 1){

        //Si inférieur à la limite
        if(targetPosition < ENTITY_SIZE) positionToReturn = ENTITY_SIZE;
        //Si supérieur à la limite
        if(targetPosition > MAP_SIZE_Y - ENTITY_SIZE) positionToReturn = MAP_SIZE_Y-ENTITY_SIZE;

    }

    return positionToReturn;
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





void computeStrategy(Dog *dogInfos, Entity *entityAround, int numberOfEntity){
  if(dogInfos->dogType == 4)
  {
    static int backPositionX;
    static int backPositionY;
    static int backPositionBeforeChaseX;
    static int backPositionBeforeChaseY;
    static int patternState;
    static int backTargetedSheepX;
    static int backTargetedSheepY;
    static int attributionID;


    //Si on est en attente de sychronisation avec les autres
    if(dogInfos->state == 0){
      srand((dogInfos->entity).ID);
     attributionID = rand()%3;
     dogInfos->state = 1;

     //Definir le point de départ (Formule)
     dogInfos->targetPositionX = ENTITY_SIZE;
     dogInfos->targetPositionY = 1300*attributionID+650;

     //Definir le point de recovery
     backPositionBeforeChaseX = ENTITY_SIZE;
     backPositionBeforeChaseY = 1300*attributionID+650;

     //Donner la direction de départ
     patternState = 3;
   }

    //Si on est en recherche de brebis
    if(dogInfos->state == 1){

      //Chercher si une brebis est visible
      for(int i = 0;i<numberOfEntity;i++){

        //Si c'est une brebis
        if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){
          //Si elle n'est pas déjà placé
          if(!(entityAround[i].positionY>= (MAP_SIZE_Y/2)-PUSHING_MARGIN && entityAround[i].positionY <= (MAP_SIZE_Y/2)+PUSHING_MARGIN)){

            //Si elle n'est pas dans la base
            if(!isInBase(entityAround[i].positionX,entityAround[i].positionY)){

              //Si elle n'est pas proche d'un bord
              if(!(entityAround[i].positionX < ENTITY_SIZE+(dogInfos->actionRange) && entityAround[i].positionX > MAP_SIZE_X-ENTITY_SIZE-(dogInfos->actionRange))){
                if(!(entityAround[i].positionY < ENTITY_SIZE+(dogInfos->actionRange) && entityAround[i].positionY > MAP_SIZE_Y-ENTITY_SIZE-(dogInfos->actionRange))){

                  //Sauvergarder la position presente sur la ligne si besoin
                  if(backPositionBeforeChaseX == 0 && backPositionBeforeChaseY == 0){
                    backPositionBeforeChaseX = (dogInfos->entity).positionX;
                    backPositionBeforeChaseY = (dogInfos->entity).positionY;

                    //Se preparer a retrouver sa postion si besoin
                    if(patternState < 4){
                      patternState--;
                      if(patternState < 0)
                        patternState = 3;
                    }
                  }

                  //La cibler
                  dogInfos->targetedSheepId = entityAround[i].ID;
                  dogInfos->state = 2;
                  backTargetedSheepX = entityAround[i].positionX;
                  backTargetedSheepY = entityAround[i].positionY;

                  //La contourner / Etape 1
                  //Si on est dans la partie haute de la map
                  if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
                    dogInfos->targetPositionX = positionClamp(backTargetedSheepX,0);
                    dogInfos->targetPositionY = positionClamp(backTargetedSheepY-(dogInfos->actionRange),1);
                  }else{  //Si on est dans la partie basse
                    dogInfos->targetPositionX = positionClamp(backTargetedSheepX,0);
                    dogInfos->targetPositionY = positionClamp(backTargetedSheepY+(dogInfos->actionRange),1);
                  }

                }
              }
            }
          }
        }
      }
    }

    //Contournement / Etape 2 / Réaligmement
    if(dogInfos->state == 2){

      //Si on a atteint la position précendente
      if(isTargetPositionReached(dogInfos)){
        printf("Here we are \n\n");
        if(isEntityInBase(dogInfos->entity)==0){
          printf("We are inside \n\n");
          int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);
          int angleRatio = entityAround[tmpIdSheep].positionY / entityAround[tmpIdSheep].positionX;

          dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX + sqrt((dogInfos->actionRange/4) / (1+pow(angleRatio,2))),0);

          //Si on est dans le haut de la map
          if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
            dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY - angleRatio*sqrt((dogInfos->actionRange/4) / (1 + pow(angleRatio,2))),1);
          }else{
            dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY + angleRatio*sqrt((dogInfos->actionRange/4) / (1 + pow(angleRatio,2))),1);
          }

          dogInfos->state = 3;
        }else{ //On est dans l'enclos donc la brebis aussi
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }

      }else{
        printf("We are not inside \n\n");
      }
    }

  //Si on est aligné
  if(dogInfos->state == 3 && isTargetPositionReached(dogInfos)){
        // Si le chien est dans l'enclos, la brebis l'est aussi
        // On peut donc arrêter la chasse et partir ailleurs

        /*
        if(sqrt(pow((dogInfos->entity).positionX,2) + pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2)) < MAP_SIZE_X/10){
        */
        if(isEntityInBase(dogInfos->entity)==1){
          printf("\n\n On repart à la chasse de manière random \n\n");
          dogInfos->state = 0;
          dogInfos->targetedSheepId = 0;
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }else{
          dogInfos->state = 4; // On passe a l'état de déplacement de brebis
          printf("\n\n Condition non vérifiée : \nDelta X carré = %f \nDelta Y carré = %f \nDistance = %f \n\n",pow((dogInfos->entity).positionX,2),pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2),sqrt(pow((dogInfos->entity).positionX,2) + pow((MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2)));
        }






    //Si on chasse une brebis
    if(dogInfos->state == 4){
      //Chercher son ID courant
      int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

      //Verifier qu'elle est toujours visible et qu'elle n'a pas atteint l'enclos
      if(tmpIdSheep != -1 && isEntityInBase(dogInfos->entity)==0){

          printf("We are still moving \n\n");
          int angleRatio = entityAround[tmpIdSheep].positionY / entityAround[tmpIdSheep].positionX;

          dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX + sqrt((dogInfos->actionRange/4) / (1+pow(angleRatio,2))),0);

          //////// Respectivement +2  -2
          //Si on est dans le haut de la map
          if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
            dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY - angleRatio*sqrt((dogInfos->actionRange/4) / (1 + pow(angleRatio,2))),1);
          }else{ // S'il est dans le bas de la map
            dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY + angleRatio*sqrt((dogInfos->actionRange/4) / (1 + pow(angleRatio,2))),1);
          }



/*
          dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX,0);
          dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY - (dogInfos->actionRange) + 2,1);

        }else{//Si il est au dessous de la limite

          dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX,0);
          dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY + (dogInfos->actionRange) - 2,1);

        }
*/
      }else{  //Si il est en position ou que qu'il a atteint le milieu

        printf("We are not moving \n\n");

        //Arreter la chasse
        dogInfos->state = 1;
        dogInfos->targetedSheepId = 0;

        //Retourner à la position de recovery
        dogInfos->targetPositionX = backPositionBeforeChaseX;
        dogInfos->targetPositionY = backPositionBeforeChaseY;
      }
    }
  }

////// A CORRIGER 
    //Si on cherche toujours une brebis
    if(dogInfos->state == 1){

      //Si on a atteint la position initiale
      if(isTargetPositionReached(dogInfos) && patternState == 3){
        //Aller vers la droite
        dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
        dogInfos->targetPositionY = (dogInfos->entity).positionY;
        patternState = 0;

        //Vider la sauvegarde de position
        backPositionBeforeChaseX = 0;
        backPositionBeforeChaseY = 0;
      }

      //Mouvement vers le bas
      if(isTargetPositionReached(dogInfos) && patternState == 0){
        //Aller vers le bas
        dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
        dogInfos->targetPositionY = (dogInfos->entity).positionY+1300;
        patternState = 1;

        //Vider la sauvegarde de position
        backPositionBeforeChaseX = 0;
        backPositionBeforeChaseY = 0;
      }

      //Mouvement vers la gauche
      if(isTargetPositionReached(dogInfos) && patternState == 1){
        //Aller vers la gauche
        dogInfos->targetPositionX = ENTITY_SIZE;
        dogInfos->targetPositionY = (dogInfos->entity).positionY;
        patternState = 2;

        //Vider la sauvegarde de position
        backPositionBeforeChaseX = 0;
        backPositionBeforeChaseY = 0;
      }

      //Mouvement vers le bas
      if(isTargetPositionReached(dogInfos) && patternState == 2){
        //Aller vers le bas
        dogInfos->targetPositionX = (dogInfos->entity).positionX;
        dogInfos->targetPositionY = (dogInfos->entity).positionY+1300;
        patternState = 3;

        //Vider la sauvegarde de position
        backPositionBeforeChaseX = 0;
        backPositionBeforeChaseY = 0;
      }

      //Si mouvements aléatoires
      if(patternState == 4 && isTargetPositionReached(dogInfos)){
        dogInfos->targetPositionX = generateRandomPosition(0,MAP_SIZE_X-ENTITY_SIZE);
        dogInfos->targetPositionY = generateRandomPosition(0,MAP_SIZE_Y-ENTITY_SIZE);
      }

      //Si on ne bouge plus (on a parcouru la map)
      if(backPositionX == (dogInfos->entity).positionX && backPositionY == (dogInfos->entity).positionY){
        //Vider la sauvegarde de position
        backPositionBeforeChaseX = 0;
        backPositionBeforeChaseY = 0;

        //Se declarer en mode aléatoire
        patternState = 4;
      }
    }

    //Si on est en sécurité
    if(dogInfos->state == 99){
      //Si on ne voit plus le mouton que l'on chassait
      if(findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId) == -1){
        //Enlever le mode de sécurité
        dogInfos->state = 1;
        dogInfos->targetedSheepId = 0;
      }
    }

/*
    //Si on ne bouge plus (après la synchorniation)
    if((dogInfos->entity).positionX == backPositionX && (dogInfos->entity).positionY == backPositionY && dogInfos->state > 1){
      //S'eloigner
      dogInfos->targetPositionX = backPositionBeforeChaseX;
      dogInfos->targetPositionY = backPositionBeforeChaseY;

      //Passer en sécurité
      dogInfos->state = 99;
      printf("Security Mode !!\n");
    }

    //Sauvergarder la position courante
    backPositionX = (dogInfos->entity).positionX;
    backPositionY = (dogInfos->entity).positionY;
*/
    }
}
