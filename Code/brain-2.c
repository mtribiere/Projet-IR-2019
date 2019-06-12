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

  if(dogInfos->dogType == 1 || dogInfos->dogType == 4)
  {
    //Si on est en recherche de brebis
    if(dogInfos->state == 0){

      int tmpIdSheep;
      //Chercher si une brebis est visible
      for(int i = 0;i<numberOfEntity;i++){

        //Si c'est une brebis
        if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){
          //Si elle n'est pas déjà placée
          int distance = sqrt(pow(entityAround[i].positionX,2)+pow((MAP_SIZE_Y/2) - entityAround[i].positionY,2));
          if(distance >= MAP_SIZE_X/10){
            //La cibler
            dogInfos->targetedSheepId = entityAround[i].ID;
            tmpIdSheep = i;

            printf("Y Sheep : %d\n",entityAround[i].positionY);
          }
        }
      }


      //Si une brebis est proche
      if(dogInfos->targetedSheepId != 0){

          //Si on est dans la partie haute de la map
          if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){

            dogInfos->targetPositionX = (dogInfos->entity).positionX;
            dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY-(dogInfos->actionRange);

          }else{//Si on est dans la partie basse

            dogInfos->targetPositionX = (dogInfos->entity).positionX;
            dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY+(dogInfos->actionRange);

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

      //Si on est aligné
      if(dogInfos->state == 3 && isTargetPositionReached(dogInfos))
        dogInfos->state = 4; // On passe a l'état de déplacement de brebis

      //Etat de déplacement de brebis
      if(dogInfos->state == 4){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

        // UNE BREBIS EST DANS L'ENCLOS SSI :
        // entityAround[i].positionX >= (MAP_SIZE_X/10)-PUSHING_MARGIN
        // ET (entityAround[i].positionY <= (MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+PUSHING_MARGIN)
        // ET (entityAround[i].positionY >= (MAP_SIZE_Y/2)-(MAP_SIZE_X/10)-PUSHING_MARGIN)

        //Si la brebis n'est pas dans l'enclos
        int distance = sqrt(pow(entityAround[tmpIdSheep].positionX,2)+pow((MAP_SIZE_Y/2) - entityAround[tmpIdSheep].positionY,2));
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

          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);


          //Partir ailleurs
          printf("Fin de la chasse\n");
        }
      }

    }
  }







  // Si on est un chien bleu : on explore aléatoirement
  // Lorsqu'on voit une brebis : on revient vers le centre jusqu'à voir au moins un Jaune et on s'arrête
  // Ce jaune partira dans la direction du bleu  (voir Si on est un jaune), lorsqu'il sera sorti du champ de vision (numberOfEntity - 1) on repart en exploration random
  if(dogInfos->dogType == 0){
    //Si on est en recherche de brebis
    if(dogInfos->state == 0){

      int tmpIdSheep;
      //Chercher si une brebis est visible
      for(int i = 0;i<numberOfEntity;i++){
        printf("\n\n Entity Around n° : %d detected \n\n",i);
        //Si c'est une brebis
        if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){
          printf("\n\n Il s'agit d'une brebis ");

          //Si elle n'est pas déjà placée
          if(entityAround[i].positionX < ((MAP_SIZE_X/10)-PUSHING_MARGIN) || entityAround[i].positionY > ((MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+PUSHING_MARGIN) || entityAround[i].positionY < ((MAP_SIZE_Y/2)-(MAP_SIZE_X/10)-PUSHING_MARGIN)){
            //La cibler
            printf("non placée \n\n");
            dogInfos->targetedSheepId = entityAround[i].ID;
            tmpIdSheep = i;

            printf("\n\n Targeted Sheep Id : %d\n\n",entityAround[i].ID);
          }else printf("déjà placée \n\n");
        }
      }


      //Si une brebis est proche
      if(dogInfos->targetedSheepId != 0 && dogInfos->targetedSheepId != (dogInfos->entity).ID){
          printf("\n\n Brebis détectée d' Id : %d\n\n",dogInfos->targetedSheepId);
          //Où que nous soyons sur la map
          dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX;
          dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY;
          if(isTargetPositionReached(dogInfos)){
            dogInfos->state = 1;
            printf("\n\n On atteint la position pour passer à l'état 1...\n\n");
          }
        }

      }
      if(dogInfos->state == 0 && dogInfos->targetedSheepId == 0){ //Si aucune brebis visible
        //Si on est arrivé a destination
        if(isTargetPositionReached(dogInfos)){
          printf("\n\n Aucune brebis détectée. Génération de nouvelles coordonnées aléatoires... \n\n");
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
      }
    }

    // Si on est en exploration
    //Si on est pas encore aligné, on s'aligne entre la brebis et le centre
      if(dogInfos->state == 1){

        printf("\n\n On rentre dans l'état 1 \n\n");
        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

        int angleRatio = (MAP_SIZE_Y/2 - entityAround[tmpIdSheep].positionY) / (MAP_SIZE_X/2 - entityAround[tmpIdSheep].positionX);
        printf("\n\n Angle ratio et tmpIDSheep déterminés \n\n");

        // Si on est à gauche du centre
        if((dogInfos->entity).positionX <= MAP_SIZE_X/2){
        dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX + sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));
      }else{
        dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX - sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));
      }
      printf("\n\n Coordonnées en X déterminées \n\n");

        //Si on est dans le haut de la map
        if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
          dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY - angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
        }else{
          dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY + angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
        }
        printf("\n\n Coordonnées en Y déterminées \n\n");

        if(isTargetPositionReached(dogInfos)){
        dogInfos->state=2; printf("\n\n Alignement des constellations \n\n");
        }
      }

      //Si on est aligné
      if(dogInfos->state == 2) {dogInfos->state = 3; // On passe a l'état de déplacement du chien à proximité du centre
      printf("\n\n Cap sur le centre \n\n");}

      //Etat de déplacement du chien à proximité du centre
      if(dogInfos->state == 3){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);
        int angleRatio = (MAP_SIZE_Y/2 - entityAround[tmpIdSheep].positionY) / (MAP_SIZE_X/2 - entityAround[tmpIdSheep].positionX);
        printf("\n\n Angle ratio et tmpIDSheep déterminés à nouveau \n\n");

        // Si on est à gauche du centre
        if((dogInfos->entity).positionX <= MAP_SIZE_X/2){
          dogInfos->targetPositionX = MAP_SIZE_X/2 - sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));
        }else{
          dogInfos->targetPositionX = MAP_SIZE_X/2 + sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));
        }

        //Si on est dans le haut de la map
        if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
          dogInfos->targetPositionY = MAP_SIZE_Y/2 - angleRatio*sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));
        }else{
          dogInfos->targetPositionY = MAP_SIZE_Y/2 + angleRatio*sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));
        }
        if(isTargetPositionReached(dogInfos)){
          // On est arrivé à proximité du centre
          printf("\n\n On est arrivé à proximité du centre \n\n");
          //Arreter le déplacement
          dogInfos->state = 4;
          dogInfos->targetedSheepId = 0;
        }
    }

    if(dogInfos->state == 4){
      // On retient le nombre de jaunes dans notre champ de vision
      int numberOfEntityTmp = numberOfEntity;

      // S'il y a au moins un jaune dans notre champ de vision
      if(numberOfEntityTmp > 0 && isYellowIn(dogInfos,entityAround,numberOfEntity)){
        // On attend qu'un jaune sorte de notre champ de vision avant de bouger
        if (numberOfEntity == numberOfEntityTmp){
          dogInfos->targetPositionX = (dogInfos->entity).positionX;
          dogInfos->targetPositionY = (dogInfos->entity).positionY;
        }else{ // Une fois que c'est fait on repart en exploration
          dogInfos->state = 0;
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);

          printf("Fin de la chasse\n");
        }
      }else{ // Sinon s'il n'y a aucun jaune dans le champ de vision
            // On attend qu'un jaune revienne pour lui indiquer la direction de la brebis
        dogInfos->targetPositionX = (dogInfos->entity).positionX;
        dogInfos->targetPositionY = (dogInfos->entity).positionY;
      }
    }
}
