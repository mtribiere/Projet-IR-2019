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












    // Faire un état initial où les jaunes vont au-dessus de l'enclos adverse
    // Changer le generateRandomPosition pour le limiter à une certaine zone au-dessus de l'enclos adverse : generatePositionAroundBase


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
          dogInfos->targetPositionX = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
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
        dogInfos->targetPositionX = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
        dogInfos->targetPositionY = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
      }

      //Si on est aligné
      if(dogInfos->state == 3 && isTargetPositionReached(dogInfos)){
           // Si le chien est dans l'enclos, la brebis l'est aussi
           // On peut donc arrêter la chasse et partir ailleurs

           if(isEntityInBase(dogInfos->entity)==1){
             printf("\n\n On repart à la chasse de manière random \n\n");
             dogInfos->state = 0;
             dogInfos->targetedSheepId = 0;
             dogInfos->targetPositionX = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
             dogInfos->targetPositionY = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
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
         dogInfos->targetPositionX = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
         dogInfos->targetPositionY = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
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
            dogInfos->targetPositionX = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
            dogInfos->targetPositionY = generatePositionAroundBase(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
          }
        }
     }

   }
 }
}
