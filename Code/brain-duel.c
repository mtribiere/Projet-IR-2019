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
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  srand((time_t)ts.tv_nsec);

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

  if(sqrt(pow(positionX,2)+pow(abs(MAP_SIZE_Y/2-positionY),2)) <= MAP_SIZE_X/10)
    toReturn = 1;
  /*if(positionX >= 0 && positionX <= MAP_SIZE_X/10 && positionY >= MAP_SIZE_Y/2-MAP_SIZE_X/10 && positionY <= MAP_SIZE_Y/2+MAP_SIZE_X/10)
    toReturn = 1;*/

  return toReturn;
}

//0 : X , 1 : Y
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
          if(entityAround[i].ID < (dogInfos->entity).ID){//Si son ID est plus petit
            dogInfos->state = 11;
          }else{//Si il est plus grand
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
        dogInfos->targetPositionY = MAP_SIZE_Y/2-MAP_SIZE_X/10;
      }
      if(dogInfos->state == 15){//Si on est celui du dessous
        dogInfos->targetPositionX = ENTITY_SIZE;
        dogInfos->targetPositionY = MAP_SIZE_Y/2+MAP_SIZE_X/10;
      }

      ////////////Point 4
      if(dogInfos->state == 6){//Si on est celui du dessus
        dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
        dogInfos->targetPositionY = MAP_SIZE_Y/2-MAP_SIZE_X/10;
      }
      if(dogInfos->state == 16){//Si on est celui du dessous
        dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
        dogInfos->targetPositionY = MAP_SIZE_Y/2+MAP_SIZE_X/10;
      }
    }
}
  //Si on est un chien Vert (Ramener les brebis sur la ligne centrale)
  if(dogInfos->dogType == 1)
  {

    //Si on est en recherche de brebis
    if(dogInfos->state == 0){

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

                  //La cibler
                  dogInfos->targetedSheepId = entityAround[i].ID;
                  dogInfos->state = 1;
                }
              }
            }
          }
        }
      }
    }

    //Si on chasse un brebis
    if(dogInfos->state != 0){
      //Chercher son ID courant
      int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

      //Verifier qu'il est toujours visible et qu'il n'as pas atteint le milieu
      if(tmpIdSheep != -1 && !isPushingPositionReached(entityAround[tmpIdSheep].positionX,entityAround[tmpIdSheep].positionY,entityAround[tmpIdSheep].positionX,MAP_SIZE_Y/2) && !isInBase(entityAround[tmpIdSheep].positionX,entityAround[tmpIdSheep].positionY)){
        //Si il est au dessus de la limite
        if(entityAround[tmpIdSheep].positionY < MAP_SIZE_Y/2){

          dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX,0);
          dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY - (dogInfos->actionRange) + 2,1);

        }else{//Si il est au dessous de la limite

          dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX,0);
          dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY + (dogInfos->actionRange) - 2,1);

        }
      }else{//Si il est en position ou que qu'il a atteint le milieu
        //Arreter la chasse
        dogInfos->state = 0;
        dogInfos->targetedSheepId = 0;

      }
    }else{//Si on cherche une brebis

      //Si on a atteint la position target ou qu'on ne bouge pas
      if(isTargetPositionReached(dogInfos) || (dogInfos->targetPositionX == 0 && dogInfos->targetPositionY == 0)){
        dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
        dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
      }

      //Eviter de percuter un autre blanc
      for(int i = 0;i<numberOfEntity;i++){
        //Si il y a un mouton
        if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){

          //L'éviter
          dogInfos->targetPositionX = positionClamp((dogInfos->entity).positionX+2*(dogInfos->actionRange),0);

          if((dogInfos->entity).positionY < MAP_SIZE_Y/2){//Si on est en haut
              dogInfos->targetPositionY = (dogInfos->entity).positionY-(dogInfos->actionRange);
          }else{ //Si on est en bas
            dogInfos->targetPositionY = positionClamp((dogInfos->entity).positionY+(dogInfos->actionRange),1);
          }

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
  }
