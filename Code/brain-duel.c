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
#include "brain-duel.h"
#include "map.h"


int generatePositionAroundBaseX(int lower,int upper){
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  srand((time_t)ts.tv_nsec);

  int positionX = (rand() % (upper - lower + 1)) + lower;

  // Si on est du côté gauche, l'enclos adverse est à droite
  if(BASE_SIDE == 1){
    while(positionX < MAP_SIZE_X - 200 || positionX > MAP_SIZE_X - 100){
      positionX = generatePositionAroundBaseX(lower, upper);
    }
  }
  if(BASE_SIDE == 2){
    while(positionX > 200 || positionX < 100){
      positionX = generatePositionAroundBaseX(lower, upper);
    }
  }

  return positionX;
}


int generatePositionAroundBaseY(int lower,int upper){
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  srand((time_t)ts.tv_nsec);

  int positionY = (rand() % (upper - lower + 1)) + lower;

  // Quel que soit notre côté
  while(positionY < MAP_SIZE_X/20 || positionY > MAP_SIZE_X/10){
      positionY = generatePositionAroundBaseY(lower, upper);
  }

  return positionY;
}

int isPushingPositionReached(int positionX,int positionY,int targetPositionX,int targetPositionY){
  int toReturn = 0;

  //Si on a atteint la destination en X
  if(positionX <= targetPositionX+PUSHING_MARGIN && positionX >= targetPositionX-PUSHING_MARGIN){
    //Si on atteint la destination en Y
    if(positionY <= targetPositionY+PUSHING_MARGIN && positionY >= targetPositionY-PUSHING_MARGIN){
      toReturn = 1;
    }
  }

  return toReturn;
}

int isInBase(int positionX,int positionY){
  int toReturn = 0;

  if (BASE_SIDE == 1){
    if(sqrt(pow(positionX,2)+pow(abs(MAP_SIZE_Y/2-positionY),2)) <= MAP_SIZE_X/10)
      toReturn = 1;
    }

  if (BASE_SIDE == 2){
    if(sqrt(pow(MAP_SIZE_X-positionX,2)+pow(abs(MAP_SIZE_Y/2-positionY),2)) <= MAP_SIZE_X/10)
      toReturn = 1;
    }

  return toReturn;
}


/***************************
\Stratégie d'extraction des brebis de l'enclos en mode duel pour Red et Cyan
****************************/
void computeStrategyDuel(Dog *dogInfos, Entity *entityAround, int numberOfEntity)
{

  if ((dogInfos->dogType == 2) || (dogInfos->dogType == 3)) {

  /*  //Infiltration vers l'enclos adverse
    if(dogInfos->state == 0){

          //Aller au meetPoint
          if(BASE_SIDE == 2) dogInfos->targetPositionX = positionClamp((1.05*(dogInfos->actionRange)),0);
          if(BASE_SIDE == 1) dogInfos->targetPositionX = positionClamp((MAP_SIZE_X - 1.05*(dogInfos->actionRange)),0);
          dogInfos->targetPositionY = MAP_SIZE_Y/2 - MAP_SIZE_X/10;

          //meetPoint atteint
          if (isTargetPositionReached(dogInfos))  dogInfos->state = 1;
        }
    if(dogInfos->state == 1){

          //Balayage
          if(BASE_SIDE == 2) dogInfos->targetPositionX = positionClamp((1.05*(dogInfos->actionRange)),0);
          if(BASE_SIDE == 1) dogInfos->targetPositionX = positionClamp((MAP_SIZE_X - 1.05*(dogInfos->actionRange)),0);
          dogInfos->targetPositionY = MAP_SIZE_Y/2 + MAP_SIZE_X/10;

          //Balayage terminé
          if (isTargetPositionReached(dogInfos)){
            for (int i; i < numberOfEntity; i++){
              if (entityAround[i].nickname[0] == 'g') {
                  dogInfos->state = 2;
                }
              }
            }


      if(dogInfos->state == 2) {

          //Contourner l'enclos
          if(BASE_SIDE == 2) dogInfos->targetPositionX = MAP_SIZE_X/10;
          if(BASE_SIDE == 1) dogInfos->targetPositionX = MAP_SIZE_X * (9/10);
          dogInfos->targetPositionY = MAP_SIZE_Y/2;

          //Contournement terminé => Go réinitialisation
          if (isTargetPositionReached(dogInfos))  dogInfos->state = 0;
        }
      } */
      int Dog_X = (dogInfos->entity).positionX;
      int Dog_Y = (dogInfos->entity).positionY;

//Infiltration vers l'enclos adverse
if(dogInfos->state == 0){

  MateID = -1;

  //Aller au meetPoint
  if(BASE_SIDE == 2) dogInfos->targetPositionX = ENTITY_SIZE + (dogInfos->actionRange);
  if(BASE_SIDE == 1) dogInfos->targetPositionX = MAP_SIZE_X - (ENTITY_SIZE + (dogInfos->actionRange));
  dogInfos->targetPositionY = MAP_SIZE_Y/2 - MAP_SIZE_X/10;

/*    if ((isTargetPositionReached(dogInfos)) && ((time(NULL) - (dogInfos->timer)) >= (TIME_UNTIL_SYNCH_CYAN))){
    dogInfos->state = 10;
    printf("Dog state = 1\n");
  } */

  //Check de mate Red/Cyan avec nous...
  for (int i = 0; i < numberOfEntity; i++){
    if((entityAround[i].nickname[0] == 'c' && entityAround[i].nickname[1] == 'y' && (entityAround[i].nickname[2] == 'a') && (dogInfos->entity).ID != entityAround[i].ID)) {
      MateID = entityAround[i].ID;
    }
  }
  if (MateID != -1) {
    if (MateID >= (dogInfos->entity).ID) {
      dogInfos->state = 20;
    }
    if (MateID < (dogInfos->entity).ID) {
      dogInfos->state = 21;
    }
    printf("DUAL MODE ACTIVATED\n");
  }
}

  // Pour le balayage des cyans
  if (dogInfos->state == 20){

    int IDofMATE = findIdOfSheep(entityAround,numberOfEntity,MateID);
    printf("\n\nMateID = %d\nIDofMATE = %d\n", MateID,IDofMATE);
    int MarginDOG_X = Dog_X - entityAround[IDofMATE].positionX;
    int MarginDOG_Y = Dog_Y - entityAround[IDofMATE].positionY;
    printf("MARGIN : X = %d | Y = %d\n",MarginDOG_X,MarginDOG_Y);

    if (((sqrt((MarginDOG_X)*(MarginDOG_X)) <= POSITION_MARGIN) && (sqrt((MarginDOG_Y)*(MarginDOG_Y)) <= POSITION_MARGIN)) && isTargetPositionReached(dogInfos)){
      printf("On initialise ?\n");
      dogInfos->targetPositionX =  positionClamp(Dog_X - (dogInfos->actionRange)/2,0);
      dogInfos->targetPositionY = Dog_Y;
      dogInfos->state = 30;
    }
  }

  if (dogInfos->state == 21){

    int IDofMATE = findIdOfSheep(entityAround,numberOfEntity,MateID);
    printf("\n\nMateID = %d\nIDofMATE = %d\n", MateID,IDofMATE);
    int MarginDOG_X = Dog_X - entityAround[IDofMATE].positionX;
    int MarginDOG_Y = Dog_Y - entityAround[IDofMATE].positionY;
    printf("MARGIN : X = %d | Y = %d\n",MarginDOG_X,MarginDOG_Y);

    if (((sqrt((MarginDOG_X)*(MarginDOG_X)) <= POSITION_MARGIN) && (sqrt((MarginDOG_Y)*(MarginDOG_Y)) <= POSITION_MARGIN)) && isTargetPositionReached(dogInfos)){
      printf("On initialise ?\n");
      dogInfos->targetPositionX = positionClamp(Dog_X + (dogInfos->actionRange)/2,0);
      dogInfos->targetPositionY = Dog_Y;
      dogInfos->state = 31;
    }
  }

  if((dogInfos->state == 30) && (isTargetPositionReached(dogInfos))){
    dogInfos->targetPositionX = positionClamp(Dog_X,0);
    dogInfos->targetPositionY = MAP_SIZE_Y/2 + MAP_SIZE_X/10;
    dogInfos->state = 40;
    printf("Pushing phase\n");
  }

  if((dogInfos->state == 31) && (isTargetPositionReached(dogInfos))){
    dogInfos->targetPositionX = positionClamp(Dog_X,0);
    dogInfos->targetPositionY = MAP_SIZE_Y/2 + MAP_SIZE_X/10;
    dogInfos->state = 41;
    printf("Pushing phase\n");
  }

  if(((isTargetPositionReached(dogInfos)) && ((dogInfos->state == 40) || (dogInfos->state == 41)))) {
    if(BASE_SIDE == 2) dogInfos->targetPositionX = positionClamp(MAP_SIZE_X/10,0);
    if(BASE_SIDE == 1) dogInfos->targetPositionX = positionClamp(MAP_SIZE_X - MAP_SIZE_X/10,0);
    dogInfos->targetPositionY = MAP_SIZE_Y/2;
    dogInfos->state = 50;
    printf("Pushing finished\n");
  }

  if((dogInfos->state == 50) && (isTargetPositionReached(dogInfos))){
    dogInfos->state = 0;
    printf("Rabattement terminé\n");
  }
}


    // Si c'est un chien jaune
    if(dogInfos->dogType == 5){

      // Si on vient d'apparaître sur la map
      if(dogInfos->state == 0){
        // Si notre enclos est à gauche, l'enclos adverse est à droite
        if(BASE_SIDE == 1){
          dogInfos->targetPositionX = MAP_SIZE_X - 100; // Marge de convention à laquelle les Red et Cyan livrent les brebis
          dogInfos->targetPositionY = MAP_SIZE_Y - MAP_SIZE_X/10; // Point aux alentours de l'enclos (affiné plus tard)
        }
        if(BASE_SIDE == 2){
          dogInfos->targetPositionX = 100; // Marge de convention à laquelle les Red et Cyan livrent les brebis
          dogInfos->targetPositionY = MAP_SIZE_Y - MAP_SIZE_X/10; // Point aux alentours de l'enclos (affiné plus tard)
        }
        if(isTargetPositionReached(dogInfos)){
          dogInfos->state = 1;
        }
      }

      //Si on est en recherche de brebis
      if(dogInfos->state == 1){

        int tmpIdSheep;
        //Chercher si une brebis est visible
        for(int i = 0;i<numberOfEntity;i++){

          //Si c'est une brebis
          if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){
            //Si elle n'est pas déjà placée
            if(isEntityInBase(entityAround[i])==0){
              //La cibler
              dogInfos->targetedSheepId = entityAround[i].ID;
              tmpIdSheep = i;

              printf("Y Sheep : %d\n",entityAround[i].positionY);
            }
          }
        }

        //Si une brebis est proche
        if(dogInfos->targetedSheepId != 0 && (isEntityInBase(entityAround[dogInfos->targetedSheepId])==0)){

         //Si on est dans la partie haute de la map
         if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){

           dogInfos->targetPositionX = (dogInfos->entity).positionX;
           dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY-(dogInfos->actionRange);

         }else{//Si on est dans la partie basse

           dogInfos->targetPositionX = (dogInfos->entity).positionX;
           dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY+(dogInfos->actionRange);

         }
         dogInfos->state = 2;

       }else{ //Si aucune brebis visible ou qu'elle est déjà dans l'enclos
        //Si on est arrivé a destination
        if(isTargetPositionReached(dogInfos)){
          dogInfos->targetPositionX = generatePositionAroundBaseX(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generatePositionAroundBaseY(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
      }
    }
    else{ // Si on est en chasse
      // Si on n'est pas dans l'enclos
      if(isEntityInBase(dogInfos->entity)==0){
        //Si on est pas encore aligné et qu'on a fini la manoeuvre précédente
        if(dogInfos->state == 2 && isTargetPositionReached(dogInfos)){

          // Cette ligne peut poser problème
          int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

          int angleRatio = entityAround[tmpIdSheep].positionY / entityAround[tmpIdSheep].positionX;

          dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX + sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));

          //Si on est dans le haut de la map
          if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
            dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY - angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
          }else{
            dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY + angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
          }

          dogInfos->state=3;
        }
      }else{ //On est dans l'enclos donc la brebis aussi
        dogInfos->targetPositionX = generatePositionAroundBaseX(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
        dogInfos->targetPositionY = generatePositionAroundBaseY(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
      }

      //Si on est aligné
      if(dogInfos->state == 3 && isTargetPositionReached(dogInfos)){
           // Si le chien est dans l'enclos, la brebis l'est aussi
           // On peut donc arrêter la chasse et partir ailleurs

           if(isEntityInBase(dogInfos->entity)==1){
             printf("\n\n On repart à la chasse de manière random \n\n");
             dogInfos->state = 0;
             dogInfos->targetedSheepId = 0;
             dogInfos->targetPositionX = generatePositionAroundBaseX(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
             dogInfos->targetPositionY = generatePositionAroundBaseY(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
           }else{ // Sinon le chien n'est pas dans l'enclos
             dogInfos->state = 4; // On passe a l'état de déplacement de brebis
             /*
             printf("\n\n Condition non vérifiée : \nDelta X carré = %f \nPosition en Y : %d \nMAP_SIZE_Y / 2 = %d \nDelta Y = %d \nDelta Y carré = %f \nSomme = %f \nDistance = %f \n\n",pow((dogInfos->entity).positionX,2),(dogInfos->entity).positionY,MAP_SIZE_Y/2,abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2),pow((dogInfos->entity).positionX,2)+pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2),sqrt(pow((dogInfos->entity).positionX,2) + pow((MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2)));
             */
           }
     }


     //Etat de déplacement de brebis
     if(dogInfos->state == 4){

       int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

       // Si brebis non trouvée
       if(tmpIdSheep == 0 || isEntityInBase(dogInfos->entity)==1){
         // On repart en chasse
         dogInfos->state = 0;
         dogInfos->targetedSheepId = 0;
         dogInfos->targetPositionX = generatePositionAroundBaseX(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
         dogInfos->targetPositionY = generatePositionAroundBaseY(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
       }else{
         //Si la brebis n'est pas dans l'enclos
         unsigned int distance = sqrt(pow(entityAround[tmpIdSheep].positionX,2)+pow(abs(MAP_SIZE_Y/2 - entityAround[tmpIdSheep].positionY),2));
         if(distance >= MAP_SIZE_X/10){

           //Continuer de pousser la brebis en réactualisant les paramètres
           int angleRatio = entityAround[tmpIdSheep].positionY / entityAround[tmpIdSheep].positionX;

           dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX + sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));

           //Si on est dans le haut de la map
           if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
             dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY - angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
           }else{
             dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY + angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
           }

         }else{ // Brebis dans l'enclos

            //Arreter la chasse
            dogInfos->state = 0;
            dogInfos->targetedSheepId = 0;

            // Partir ailleurs
            dogInfos->targetPositionX = generatePositionAroundBaseX(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
            dogInfos->targetPositionY = generatePositionAroundBaseY(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
          }
        }
      }
    }
   }
  }
