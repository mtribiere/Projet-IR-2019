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

#define TIME_UNTIL_SYNCH 15

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

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int partition (int arr[], int low, int high)
{
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high- 1; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

//Retourne l'ID attribué
int purpleSynch(Entity *entityAround,int numberOfEntity,int idToFind){
  int toReturnID = 0;

  /////////Compter le nombre de Violet
  int purpleCount = 0;
  //Pour toutes le entité présentes
  for(int i = 0;i<numberOfEntity;i++){
    //Si c'est un violet
    if(entityAround[i].nickname[0] == 'p' && entityAround[i].nickname[1] == 'u' && entityAround[i].nickname[2] == 'r')
      purpleCount++;
  }

  ////////Remplir le tableau d'IDs
  //Augmenter le tableau d'ID pour le notre
  purpleCount++;

  //Déclarer le tableau des ID
  int *purpleIDs = malloc(sizeof(int)*(purpleCount+1));
  int purpleIndex = 0;

  //Pour toutes le entité présentes
  for(int i = 0;i<numberOfEntity;i++){
    //Si c'est un violet
    if(entityAround[i].nickname[0] == 'p' && entityAround[i].nickname[1] == 'u' && entityAround[i].nickname[2] == 'r'){
      purpleIDs[purpleIndex] = entityAround[i].ID;
      purpleIndex++;
    }
  }
  //Inserer notre ID
  purpleIDs[purpleCount-1] = idToFind;

  /////////Trier le tableau d'ID (tri bulle)
  quickSort(purpleIDs,0,purpleCount-1);

  /////DEBUG
  printf("IDs purple : ");
  for(int i = 0;i<purpleCount;i++)
    printf("%d;",purpleIDs[i]);
  printf("\n");

  /////////Trouver l'ID attribué
  for(int i = 0;i<purpleCount;i++){
      //Si on a trouvé la valeur
      if(purpleIDs[i] == idToFind){
        toReturnID = i;
      }
  }

  //Si l'ID est trop grand
  if(1300*toReturnID+650 >= MAP_SIZE_Y-ENTITY_SIZE){
    toReturnID = 0;
  }

  ////////Liberer le tableau d'IDs
  free(purpleIDs);


  return toReturnID;
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
  if(dogInfos->dogType == 1){

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
        if(entityAround[i].nickname[0] == 'g' && entityAround[i].nickname[1] == 'r' && entityAround[i].nickname[2] == 'e' && entityAround[i].ID != (dogInfos->entity).ID){
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

    //Si un partenaire a été trouvé
    if(dogInfos->state == 1 || dogInfos->state == 11){

      //Si on a atteint le meetPoint
      //En X
      if((dogInfos->entity).positionX>=MAP_SIZE_X-ENTITY_SIZE-POSITION_MARGIN){
        //En Y
        if((dogInfos->entity).positionY <= MAP_SIZE_Y/2+POSITION_MARGIN && (dogInfos->entity).positionY >= MAP_SIZE_Y/2-POSITION_MARGIN){

          //Si le partenaire est en place
          int tmpId = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

          //Si pas d'erreur de synchronisation
          if(tmpId != -1){
            printf("Waiting parterner\n");
            //En X
            if(entityAround[tmpId].positionX >= MAP_SIZE_X-ENTITY_SIZE-POSITION_MARGIN){
              //En Y
              if(entityAround[tmpId].positionY <= MAP_SIZE_Y/2+POSITION_MARGIN && entityAround[tmpId].positionY >= MAP_SIZE_Y/2-POSITION_MARGIN){
                //Commencer le pattern
                (dogInfos->state)++;

                //Ne plus bouger
                dogInfos->targetPositionX = (dogInfos->entity).positionX;
                dogInfos->targetPositionY = (dogInfos->entity).positionY;
              }
            }
          }else{//Erreur de synchronisation
            dogInfos->state = 0;
            dogInfos->targetedSheepId = 0;
          }
        }
      }
    }
    ////////////////Point 2 / Spécial
    if(dogInfos->state == 4 || dogInfos->state == 14){//Si on est celui du dessus
      //Si on a atteint la position X voulu
      if((dogInfos->entity).positionX <= MAP_SIZE_X/10){

        //Ne plus bouger et passer au point suivant
        dogInfos->targetPositionX = (dogInfos->entity).positionX;
        dogInfos->targetPositionY = (dogInfos->entity).positionY;

        //Passer à l'etat suivant
        (dogInfos->state)++;

      }else{//Si on a pas atteint la position
        dogInfos->targetPositionX = (dogInfos->entity).positionX -20;//Vitesse de déplacment
        dogInfos->targetPositionY = (dogInfos->entity).positionY;
      }
    }


    if(dogInfos->state != 0 && dogInfos->state != 1 && dogInfos->state != 11 && dogInfos->state != 4 && dogInfos->state != 14 && isTargetPositionReached(dogInfos)){
      //Passer au point suivant
      (dogInfos->state)++;

      //Boucler
      if(dogInfos->state == 8){
        dogInfos->state = 0;
        dogInfos->targetedSheepId = 0;
      }
      if(dogInfos->state == 18){
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

      //////////////Point 3
      if(dogInfos->state == 6){//Si on est celui du dessus
        dogInfos->targetPositionX = ENTITY_SIZE;
        dogInfos->targetPositionY = ENTITY_SIZE;
      }
      if(dogInfos->state == 16){//Si on est celui du dessous
        dogInfos->targetPositionX = ENTITY_SIZE;
        dogInfos->targetPositionY = MAP_SIZE_Y-ENTITY_SIZE;
      }

      ////////////Point 4
      if(dogInfos->state == 7){//Si on est celui du dessus
        dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
        dogInfos->targetPositionY = ENTITY_SIZE;
      }
      if(dogInfos->state == 17){//Si on est celui du dessous
        dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
        dogInfos->targetPositionY = MAP_SIZE_Y-ENTITY_SIZE;
      }
    }
}
  //Si on est un chien Violet (Ramener les brebis sur la ligne centrale)
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
      //Aller au meetPoint
      dogInfos->targetPositionX = MAP_SIZE_X/2;
      dogInfos->targetPositionY = MAP_SIZE_Y/2;

      //Si le timer arrive à la limite
      if(time(NULL)-(dogInfos->timer) >= TIME_UNTIL_SYNCH){
        //Se synchroniser
        attributionID = purpleSynch(entityAround,numberOfEntity,(dogInfos->entity).ID);
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
    }

    if(dogInfos->state > 0){
      printf("Attribution ID : %d\n",attributionID);
      printf("Pattern State : %d\n",patternState);
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
                  //Si elle est a notre droite
                  if(backTargetedSheepX >= (dogInfos->entity).positionX){
                    dogInfos->targetPositionX = backTargetedSheepX-3*(dogInfos->actionRange);
                    dogInfos->targetPositionY = (dogInfos->entity).positionY;
                  }else{ // Si elle est a notre gauche
                    dogInfos->targetPositionX = backTargetedSheepX+3*(dogInfos->actionRange);
                    dogInfos->targetPositionY = (dogInfos->entity).positionY;
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

        //Passer a l'état suivant
        (dogInfos->state) = 3;

        //Si il est au dessus de la limite
        if(backTargetedSheepY < MAP_SIZE_Y/2){

          dogInfos->targetPositionX = positionClamp(backTargetedSheepX,0);
          dogInfos->targetPositionY = positionClamp(backTargetedSheepY - (dogInfos->actionRange) - 100,1);

        }else{//Si il est au dessous de la limite

          dogInfos->targetPositionX = positionClamp(backTargetedSheepX,0);
          dogInfos->targetPositionY = positionClamp(backTargetedSheepY + (dogInfos->actionRange) + 100,1);

      }
    }
  }

  //Finir le contournement
  if(dogInfos->state == 3){
    //Si on a atteint la position precedente
    if(isTargetPositionReached(dogInfos))
      (dogInfos->state) = 4;

  }

    //Si on chasse un brebis
    if(dogInfos->state == 4){
      //Chercher son ID courant
      int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

      //Verifier qu'il est toujours visible et qu'il n'as pas atteint le milieu
      if(tmpIdSheep != -1 && !isPushingPositionReached(entityAround[tmpIdSheep].positionX,entityAround[tmpIdSheep].positionY,entityAround[tmpIdSheep].positionX,MAP_SIZE_Y/2) && !isInBase(entityAround[tmpIdSheep].positionX,entityAround[tmpIdSheep].positionY)){
        //Si il est au dessus de la limite
        if(entityAround[tmpIdSheep].positionY < MAP_SIZE_Y/2){

          dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX,0);
          dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY - (dogInfos->actionRange) + 4,1);

        }else{//Si il est au dessous de la limite

          dogInfos->targetPositionX = positionClamp(entityAround[tmpIdSheep].positionX,0);
          dogInfos->targetPositionY = positionClamp(entityAround[tmpIdSheep].positionY + (dogInfos->actionRange) - 4,1);

        }
      }else{//Si il est en position ou que qu'il a atteint le milieu
        //Arreter la chasse
        dogInfos->state = 1;
        dogInfos->targetedSheepId = 0;

        //Retourner à la position de recovery
        dogInfos->targetPositionX = backPositionBeforeChaseX;
        dogInfos->targetPositionY = backPositionBeforeChaseY;
      }
    }

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

/*
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
      }*/
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
  }
}
