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
#include "map.h"


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
\Strategie pour les chiens Verts et Violets seulement
****************************/

void computeStrategy(Dog *dogInfos, Entity *entityAround, int numberOfEntity)
{
  // Dans brain-2 : Random Strategy
  // On fait des teams de chiens
  // Genre bleu ET jaune avec danse
  // Verts et violets tous seuls
  //Rouge et Cyan non traités #useless

  // RANDOM STRATEGIE HYPER AMELIOREE
  // On fait balayer aux violets leurs lignes d'apparition comme pensé au départ
  // Lorsque ligne balayée, on choisit une position random
  // On recommence à balayer la ligne suivante (désignée par la position random)

  //Si on est un chien Vert ou Violet (Ramener les brebis directement à l'enclos)

  // FREINE LES VERTS STP ILS SONT TROP RAPIDES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if(dogInfos->dogType == 4)
  {
    //Si on est en recherche de brebis
    if(dogInfos->state == 0){

      int tmpIdSheep;
      //Chercher si une brebis est visible
      for(int i = 0;i<numberOfEntity;i++){

        //Si c'est une brebis
        if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){
          //Si elle n'est pas déjà placée
          if(isEntityInBase(entityAround[i])==0){

            /*
            //Si elle n'est pas proche d'un bord
            if(!(entityAround[i].positionX < ENTITY_SIZE+(dogInfos->actionRange) && entityAround[i].positionX > MAP_SIZE_X-ENTITY_SIZE-(dogInfos->actionRange))){
              if(!(entityAround[i].positionY < ENTITY_SIZE+(dogInfos->actionRange) && entityAround[i].positionY > MAP_SIZE_Y-ENTITY_SIZE-(dogInfos->actionRange))){
            */

                //La cibler
                dogInfos->targetedSheepId = entityAround[i].ID;
                tmpIdSheep = i;

                printf("Y Sheep : %d\n",entityAround[i].positionY);
              /*
              }
            }
            */
            // Faire des else
           }
          }
        }

      //Si une brebis est proche
      if(dogInfos->targetedSheepId != 0 && (isEntityInBase(entityAround[dogInfos->targetedSheepId])==0)){

          //Si on est dans la partie haute de la map
          if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){

            dogInfos->targetPositionX = (dogInfos->entity).positionX;
            dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY-(dogInfos->actionRange),1);

          }else{//Si on est dans la partie basse

            dogInfos->targetPositionX = (dogInfos->entity).positionX;
            dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY+(dogInfos->actionRange),1);

        }
        dogInfos->state = 2;
      }else{ //Si aucune brebis visible ou qu'elle est déjà dans l'enclos
        //Si on est arrivé a destination
        if(isTargetPositionReached(dogInfos)){
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
      }
    }else{ // Si on est en chasse
      if(isEntityInBase(dogInfos->entity)==0){
      //Si on est pas encore aligné et qu'on a fini la manoeuvre précédente
      if(dogInfos->state == 2 && isTargetPositionReached(dogInfos)){

        // Trouver à quelle entité la brebis ciblée correspond
        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

        int angleRatio = entityAround[tmpIdSheep].positionY / entityAround[tmpIdSheep].positionX;

        dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX + sqrt(dogInfos->actionRange / (1+pow(angleRatio,2))),0);

        //Si on est dans le haut de la map
        if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
          dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY - angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2))),1);
        }else{
          dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY + angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2))),1);
        }

         dogInfos->state=3;
      }
    }else{ //On est dans l'enclos donc la brebis aussi
        dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
        dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
    }

      //Si on est aligné
            if(dogInfos->state == 3 && isTargetPositionReached(dogInfos)){
            // Si le chien est dans l'enclos, la brebis l'est aussi
            // On peut donc arrêter la chasse et partir ailleurs

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
      }


      //Etat de déplacement de brebis
      if(dogInfos->state == 4){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

        ////// A MODIFIER (SAUVEGARDER POSITIONS)
        // Si brebis non trouvée
        if(tmpIdSheep == 0 || isEntityInBase(dogInfos->entity)==1){
          // On repart en chasse
          dogInfos->state = 0;
          dogInfos->targetedSheepId = 0;
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }

        else{
        //Si la brebis n'est pas dans l'enclos
        unsigned int distance = sqrt(pow(entityAround[tmpIdSheep].positionX,2)+pow(abs(MAP_SIZE_Y/2 - entityAround[tmpIdSheep].positionY),2));
        if(distance >= MAP_SIZE_X/10){

            //Continuer de pousser la brebis en réactualisant les paramètres
            int angleRatio = entityAround[tmpIdSheep].positionY / entityAround[tmpIdSheep].positionX;

            dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX + sqrt(dogInfos->actionRange / (1+pow(angleRatio,2))),0);

            //Si on est dans le haut de la map
            if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
              dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY - angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2))),1);
            }else{
              dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY + angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2))),1);
            }

        }else{ // Brebis dans l'enclos

          //Arreter la chasse
          dogInfos->state = 0;
          dogInfos->targetedSheepId = 0;

          // Partir ailleurs
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
        }
      }

    }
  }

}
