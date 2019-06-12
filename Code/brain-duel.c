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

int findIdOfSheep(Entity *entityAround,int numberOfEntity,int idToFind){

  int idToReturn = -1;
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
/* La stratégie qu'on utlisera dans le brain duel sera la suivante :
    Utilisons les verts et violets de manière classique : dés qu'ils voient une brebis, il la ramène dans l'enclos
    Pour piquer les brbeis dans l'enclos advers, on va utiliser un duo Cyan (ou rouge) et Jaune :
      Les cyan et rouges sortirons les brebis de l'enclos et les jaunes ramèneront les brebis à l'autre enclos (en contournant le reste de la map")
      Il s'agit d'une stratégie assez brut mais l'objectif ici est de faire qqc de fonctionnel : Récuperer les brebis d'un enclos. */



      //Si on est un chien Violet (Simple carré haut ou bas)
      if(dogInfos->dogType == 4){

        //Si un partenaire n'a pas été trouvé
        if(dogInfos->targetedSheepId == 0){

          //Aller au meetPoint
          dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
          dogInfos->targetPositionY = MAP_SIZE_Y/2;
        }

        //Etat initial (attente de trouver le partenaire)
        if(dogInfos->state == 0){

          //Detecter si il y a un partenaire
          for(int i = 0;i<numberOfEntity;i++){
            //Si c'est un partenaire
            if(entityAround[i].nickname[0] == 'p' && entityAround[i].nickname[1] == 'u' && entityAround[i].nickname[2] == 'r' && entityAround[i].ID != (dogInfos->entity).ID){
              //Le cibler
              dogInfos->targetedSheepId = entityAround[i].ID;

              //Determiner les rôles
              if(entityAround[i].positionY < (dogInfos->entity).positionY){//Si il est au dessus
                dogInfos->state = 11;
              }else{//Si est en dessous
                dogInfos->state = 1;
              }

            }
          }
        }

        //Si on a atteint le meetPoint est que un partenaire a été trouvé
        if((dogInfos->state == 1 || dogInfos->state == 11) && isTargetPositionReached(dogInfos)){
          //Si le partenaire est en place
          int tmpId = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

          //En X
          if(entityAround[tmpId].positionX >= MAP_SIZE_X-ENTITY_SIZE-POSITION_MARGIN && entityAround[tmpId].positionX <= MAP_SIZE_X-ENTITY_SIZE+POSITION_MARGIN){
            //En Y
            if(entityAround[tmpId].positionY >= MAP_SIZE_Y/2-POSITION_MARGIN && entityAround[tmpId].positionY <= MAP_SIZE_Y/2+POSITION_MARGIN){
              //Commencer le pattern
              (dogInfos->state)++;
            }
          }
        }

        if(dogInfos->state != 0 && dogInfos->state != 1 && dogInfos->state != 11 && isTargetPositionReached(dogInfos)){
          //Passer au point suivant
          (dogInfos->state)++;

          //Boucler
          if(dogInfos->state == 7){
            dogInfos->state = 0;
            dogInfos->targetedSheepId = 0;
          }
          if(dogInfos->state == 17){
            dogInfos->state = 0;
            dogInfos->targetedSheepId = 0;
          }

          /////////////////Point 1
          if(dogInfos->state == 3){//Si on est celui du dessus
            dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
            dogInfos->targetPositionY = MAP_SIZE_Y/2-(dogInfos->actionRange);
          }
          if(dogInfos->state == 13){//Si on est celui du dessous
            dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
            dogInfos->targetPositionY = MAP_SIZE_Y/2+(dogInfos->actionRange);
          }

          ////////////////Point 2
          if(dogInfos->state == 4){//Si on est celui du dessus
            dogInfos->targetPositionX = ENTITY_SIZE;
            dogInfos->targetPositionY = MAP_SIZE_Y/2-(dogInfos->actionRange);
          }
          if(dogInfos->state == 14){//Si on est celui du dessous
            dogInfos->targetPositionX = ENTITY_SIZE;
            dogInfos->targetPositionY = MAP_SIZE_Y/2+(dogInfos->actionRange);
          }


          //////////////Point 3
          if(dogInfos->state == 5){//Si on est celui du dessus
            dogInfos->targetPositionX = ENTITY_SIZE;
            dogInfos->targetPositionY = ENTITY_SIZE;
          }
          if(dogInfos->state == 15){//Si on est celui du dessous
            dogInfos->targetPositionX = ENTITY_SIZE;
            dogInfos->targetPositionY = MAP_SIZE_Y-ENTITY_SIZE;
          }

          ////////////Point 4
          if(dogInfos->state == 6){//Si on est celui du dessus
            dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
            dogInfos->targetPositionY = ENTITY_SIZE;
          }
          if(dogInfos->state == 16){//Si on est celui du dessous
            dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
            dogInfos->targetPositionY = MAP_SIZE_Y-ENTITY_SIZE;
          }
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


              dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX;
              dogInfos->targetPositionY = (dogInfos->entity).positionY;

             dogInfos->state=3;

          }

          //Si on est aligné
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
                  dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY-(dogInfos->actionRange)+1;
                }else{
                  dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY+(dogInfos->actionRange)-1;
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

  if(dogInfos->dogType == 2 || dogInfos->dogType == 3) {

    //Infiltration vers l'enclos adverse
    if(dogInfos->state == 0){

      //Aller au meetPoint
      dogInfos->targetPositionX = MAP_SIZE_X/28;
      dogInfos->targetPositionY = MAP_SIZE_Y/2;

      if (isTargetPositionReached(dogInfos)){
        dogInfos->state = 1;
      }
    }

    //Detection des sheeps
    if(dogInfos->state == 1 && isTargetPositionReached(dogInfos)){

      int tmpIdSheep;
      //Chercher si une brebis est visible
      for(int i = 0;i<numberOfEntity;i++){

        //Si c'est une brebis
        if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){

          //Si elle est toujours dans l'enclos
          if( (entityAround[i].positionX)*(entityAround[i].positionX) + (entityAround[i].positionY-MAP_SIZE_Y)*(entityAround[i].positionY-MAP_SIZE_Y) <= (MAP_SIZE_X/10) ){

            //La cibler
            dogInfos->targetedSheepId = entityAround[i].ID;
            tmpIdSheep = i;

            printf("Y Sheep : %d\n",entityAround[i].positionY);

          }
        }
      }

      if(dogInfos->targetedSheepId != 0){

        //Se positionnner de telle sorte qu'on puisse faire sortir la brebis par le haut
        dogInfos->targetPositionX = (dogInfos->entity).positionX;
        dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY+(dogInfos->actionRange);
      }
      dogInfos->state = 2;
    }

    //Si on est pas encore aligné et qu'on a fini la manoeuvre précédente
      if(dogInfos->state == 2 && isTargetPositionReached(dogInfos)){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);


          dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX;
          dogInfos->targetPositionY = (dogInfos->entity).positionY;

         dogInfos->state=3;
    }


    //Si on est aligné
    if(dogInfos->state == 3 && isTargetPositionReached(dogInfos))
      dogInfos->state = 4; // On passe a l'état de déplacement de brebis

    //Etat de déplacement de brebis
    if(dogInfos->state == 4){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

        if( (entityAround[tmpIdSheep].positionX)*(entityAround[tmpIdSheep].positionX) + (entityAround[tmpIdSheep].positionY-MAP_SIZE_Y)*(entityAround[tmpIdSheep].positionY-MAP_SIZE_Y) <= (MAP_SIZE_X/10) ){

          //Continuer de pousser la brebis
          dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX;
          dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY+(dogInfos->actionRange)-1;

          }

      } else { // Brebis en position

        //Arreter la chasse
        dogInfos->state = 0;
        dogInfos->targetedSheepId = 0;

        //Partir ailleurs
        printf("Fin de la chasse\n");
      }
    }

    if(dogInfos->dogType == 5){

    }
}
