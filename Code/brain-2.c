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


int min(int a, int b){
  int toReturn=a;
  if(b < a){
    toReturn = b;
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
          unsigned int distance = sqrt(pow(entityAround[i].positionX,2)+pow((MAP_SIZE_Y/2) - entityAround[i].positionY,2));
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
    }else{ // Si on est en chasse

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
      if(dogInfos->state == 3 && isTargetPositionReached(dogInfos)){
      if(sqrt(pow((dogInfos->entity).positionX,2) + pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2)) < MAP_SIZE_X/10){
        // Si le chien est dans l'enclos, la brebis l'est aussi
        // On peut donc arrêter la chasse et partir ailleurs
        dogInfos->state = 0;
        dogInfos->targetedSheepId = 0;
        dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
        dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
      }
      else{
        dogInfos->state = 4; // On passe a l'état de déplacement de brebis
        printf("\n\n Condition non vérifiée : \nDelta X carré = %f \nPosition en Y : %d \nMAP_SIZE_Y / 2 = %d \nDelta Y = %d \nDelta Y carré = %f \nSomme = %f \nDistance = %f \n\n",pow((dogInfos->entity).positionX,2),(dogInfos->entity).positionY,MAP_SIZE_Y/2,abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2),pow((dogInfos->entity).positionX,2)+pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2),sqrt(pow((dogInfos->entity).positionX,2) + pow((MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2)));
      }
    }

      //Etat de déplacement de brebis
      if(dogInfos->state == 4){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);
        // Si brebis non trouvée
        if(tmpIdSheep == 0){
          // On repart en chasse
          dogInfos->state = 0;
          dogInfos->targetedSheepId = 0;
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }

        //Si la brebis n'est pas dans l'enclos
        unsigned int distance = sqrt(pow(entityAround[tmpIdSheep].positionX,2)+pow((MAP_SIZE_Y/2) - entityAround[tmpIdSheep].positionY,2));
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
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
        if(sqrt(pow((dogInfos->entity).positionX,2) + pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2)) < MAP_SIZE_X/10){
          // Si le chien est dans l'enclos, la brebis l'est aussi
          // On peut donc arrêter la chasse et partir ailleurs
          dogInfos->state = 0;
          dogInfos->targetedSheepId = 0;
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
        else{
          printf("\n\n Condition non vérifiée : \nDelta X carré = %f \nPosition en Y : %d \nMAP_SIZE_Y / 2 = %d \nDelta Y = %d \nDelta Y carré = %f \nSomme = %f \nDistance = %f \n\n",pow((dogInfos->entity).positionX,2),(dogInfos->entity).positionY,MAP_SIZE_Y/2,abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2),pow((dogInfos->entity).positionX,2)+pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2),sqrt(pow((dogInfos->entity).positionX,2) + pow((MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2)));
        }
      }

    }
  }







  // Si on est un chien bleu : on explore aléatoirement
  // Lorsqu'on voit une brebis : on revient vers le centre jusqu'à voir au moins un Jaune et on s'arrête
  // Ce jaune partira dans la direction du bleu  (voir Si on est un jaune), lorsqu'il sera sorti du champ de vision (numberOfEntity - 1) on repart en exploration random
  if(dogInfos->dogType == 0){

    //Si on est immobile, on se lance en mouvement
    if(dogInfos->targetPositionX == 0 && dogInfos->targetPositionY == 0){
    dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
    dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
    }

    //Si on est en recherche de brebis
    if(dogInfos->state == 0){

      int tmpIdSheep;
      //Chercher si une brebis est visible
      for(int i = 0;i<numberOfEntity;i++){

        //Si c'est une brebis
        if(entityAround[i].nickname[0] == 'b' && entityAround[i].nickname[1] == 'o' && entityAround[i].nickname[2] == 't'){

          //Si elle n'est pas déjà placée
          if(entityAround[i].positionX < ((MAP_SIZE_X/10)-PUSHING_MARGIN) || entityAround[i].positionY > ((MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+PUSHING_MARGIN) || entityAround[i].positionY < ((MAP_SIZE_Y/2)-(MAP_SIZE_X/10)-PUSHING_MARGIN)){
            //La cibler
            dogInfos->targetedSheepId = entityAround[i].ID;
            tmpIdSheep = i;
          }
        }
      }


      //Si une brebis est proche
      if(dogInfos->targetedSheepId != 0 && dogInfos->targetedSheepId != (dogInfos->entity).ID){
          //Où que nous soyons sur la map
          dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX;
          dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY;
          if(isTargetPositionReached(dogInfos)){
            dogInfos->state = 1;
          }
        }

      }
      if(dogInfos->state == 0 && dogInfos->targetedSheepId == 0){ //Si aucune brebis visible
        //Si on est arrivé a destination
        if(isTargetPositionReached(dogInfos)){
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
      }

    // Si on est en exploration
    //Si on est pas encore aligné, on s'aligne entre la brebis et le centre (si jamais rayon d'action non nul, sinon sur brebis)
      if(dogInfos->state == 1){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);
        int angleRatio;

        if(entityAround[tmpIdSheep].positionX == MAP_SIZE_X/2){
          angleRatio = 1;
        }else{
          angleRatio = (MAP_SIZE_Y/2 - entityAround[tmpIdSheep].positionY) / (MAP_SIZE_X/2 - entityAround[tmpIdSheep].positionX);
        }
        // Si on est à gauche du centre
        if((dogInfos->entity).positionX <= MAP_SIZE_X/2){
        dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX + sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));
      }else{
        dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX - sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));
      }

        //Si on est dans le haut de la map
        if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
          dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY - angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
        }else{
          dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY + angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
        }

        if(isTargetPositionReached(dogInfos)) dogInfos->state=2;
      }

      //Si on est aligné
      if(dogInfos->state == 2) dogInfos->state = 3; // On passe a l'état de déplacement du chien à proximité du centre

      //Etat de déplacement du chien à proximité du centre
      if(dogInfos->state == 3){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);
        int angleRatio;

        if(entityAround[tmpIdSheep].positionX == MAP_SIZE_X/2){
          angleRatio = 1;
        }else{
          angleRatio = (MAP_SIZE_Y/2 - entityAround[tmpIdSheep].positionY) / (MAP_SIZE_X/2 - entityAround[tmpIdSheep].positionX);
        }

        // On prend un rayon de 400 (deux tiers du champ de vision d'un jaune) pour être repéré des jaunes sans atteindre le centre
        // Si on est à gauche du centre
        if((dogInfos->entity).positionX <= MAP_SIZE_X/2){
          dogInfos->targetPositionX = MAP_SIZE_X/2 - sqrt(400 / (1+pow(angleRatio,2)));
        }else{
          dogInfos->targetPositionX = MAP_SIZE_X/2 + sqrt(400 / (1+pow(angleRatio,2)));
        }

        //Si on est dans le haut de la map
        if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
          dogInfos->targetPositionY = MAP_SIZE_Y/2 - angleRatio*sqrt(400 / (1+pow(angleRatio,2)));
        }else{
          dogInfos->targetPositionY = MAP_SIZE_Y/2 + angleRatio*sqrt(400 / (1+pow(angleRatio,2)));
        }

        if(isTargetPositionReached(dogInfos)){
          // On est arrivé à proximité du centre
          //Arreter le déplacement
          dogInfos->state = 4;
          dogInfos->targetedSheepId = 0;
        }
    }


    if(dogInfos->state == 4){
      // S'il y a au moins un jaune dans notre champ de vision
      if(numberOfEntity > 0 && isYellowIn(dogInfos,entityAround,numberOfEntity)){

        // Pour attendre que le jaune parte
        // On va se rapprocher du centre en divisant par deux l'écart entre le centre et nos coordonnees
        dogInfos->targetPositionX = (dogInfos->entity).positionX + ((dogInfos->entity).positionX - MAP_SIZE_X/2)/2;
        dogInfos->targetPositionY = (dogInfos->entity).positionY - ((dogInfos->entity).positionY - MAP_SIZE_Y/2)/2;
      }

      if(isTargetPositionReached(dogInfos)) dogInfos->state = 5;
/*
      }else{ // Sinon s'il n'y a aucun jaune dans le champ de vision
            // On attend qu'un jaune revienne pour lui indiquer la direction de la brebis
        dogInfos->targetPositionX = (dogInfos->entity).positionX;
        dogInfos->targetPositionY = (dogInfos->entity).positionY;
      }*/

  }

  if(dogInfos->state == 5){

      // On est arrivé encore plus à proximité du centre
      // Une fois que c'est fait on repart en exploration
      dogInfos->state = 0;
      dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
      dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);

    }
  }










    // Si on est un chien jaune : on va au centre de la map et on attend
    // Lorsqu'on voit un bleu : on sélectionne un jaune pour bouger
    // Ce jaune partira dans la direction du bleu jusqu'à rencontre une brebis ou arriver au bout de la map
    // S'il trouve une brebis, il la ramène vers l'enclos puis revient au centre Sinon il revient directemement au centre

    if(dogInfos->dogType == 5){

      // Création de variables statiques utilisees plus tard pour ne pas les remttre à 0 tout le temps
      static int deplacement_sur_x;
      static int deplacement_sur_y;
      static int idYellowToMove;

      // Dès qu'on apparait sur la map :
      if(dogInfos->state == 0){
        // On va vers le centre de la map
          dogInfos->targetPositionX = MAP_SIZE_X/2;
          dogInfos->targetPositionY = MAP_SIZE_Y/2;
          if(isTargetPositionReached(dogInfos)) dogInfos->state = 1;

          // Initialisation des variables
          deplacement_sur_x = 0;
          deplacement_sur_y = 0;
          idYellowToMove = 0;
      }

      if(dogInfos->state == 1){

        int isBrebisNear = 0;
        // Se réinitialise lorsqu'on revient de l'état > 1

        //Chercher quel jaune va bouger et de combien
        for(int idAround = 0;idAround<numberOfEntity;idAround++){
          //Si c'est un chien bleu, on retient sa directon
          if(entityAround[idAround].nickname[0] == 'b' && entityAround[idAround].nickname[1] == 'l' && entityAround[idAround].nickname[2] == 'u' && entityAround[idAround].nickname[3]=='e'){
            if(deplacement_sur_x == 0 && deplacement_sur_y == 0){ // Si le jaune ne connait pas déjà une valeur de destination, on lui en donne une
              deplacement_sur_x = entityAround[idAround].positionX - MAP_SIZE_X/2;
              deplacement_sur_y = entityAround[idAround].positionY - MAP_SIZE_Y/2;
            }
          }
          // Sinon si c'est un chien jaune, on choisit le jaune à déplacer
          if (entityAround[idAround].nickname[0] == 'y' && entityAround[idAround].nickname[1] == 'e' && entityAround[idAround].nickname[2] == 'l' && entityAround[idAround].nickname[3]=='l' && entityAround[idAround].nickname[4] == 'o' && entityAround[idAround].nickname[5]=='w'){
            idYellowToMove = min(entityAround[idAround].ID,(dogInfos->entity.ID));
          }
          if (entityAround[idAround].nickname[0] == 'b' && entityAround[idAround].nickname[1] == 'o' && entityAround[idAround].nickname[2] == 't'){
            isBrebisNear++;
          }
        }

        if((deplacement_sur_x != 0 || deplacement_sur_y != 0) && (dogInfos->entity).ID == idYellowToMove){
            if(isTargetPositionReached(dogInfos)){
                dogInfos->targetPositionX += deplacement_sur_x;
                dogInfos->targetPositionY += deplacement_sur_y;
            }
          }
          if (isBrebisNear != 0){  // Il y a au moins une brebis à proximité à ramener à l'enclos
              dogInfos->state = 2;

              int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);
              dogInfos->targetedSheepId = tmpIdSheep;


              //Si la brebis n'est pas dans l'enclos
              unsigned int distance = sqrt(pow(entityAround[tmpIdSheep].positionX,2)+pow((MAP_SIZE_Y/2) - entityAround[tmpIdSheep].positionY,2));;
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

                // Revenir vers le centre
                dogInfos->targetPositionX = MAP_SIZE_X/2;
                dogInfos->targetPositionY = MAP_SIZE_Y/2;

            }
        }
    }


        // On est dans l'état où on a détecté une brebis à proximité
        // On va donc la ramener à l'enclos, sauf si quelqu'un s'en charge déjà
        if(dogInfos->state == 2){
          // Avant toute chose, on peut réinitialiser les variables statiques pour les autres jaunes
          deplacement_sur_x = 0;
          deplacement_sur_y = 0;
          idYellowToMove = 0;


          int sheepToHunt = 1;
          //Eviter de prendre en chasse la brebis d'un autre jaune
          for(int i = 0;i<numberOfEntity;i++){
            //Si il y a un jaune
            if(entityAround[i].nickname[0] == 'y' && entityAround[i].nickname[1] == 'e' && entityAround[i].nickname[2] == 'l'){
              //Ne pas prendre en chasse la brebis
              sheepToHunt = 0;
            }
          }

          if(sheepToHunt != 0){

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
      }

        //Si on est aligné
        if(dogInfos->state == 3 && isTargetPositionReached(dogInfos))
          dogInfos->state = 4; // On passe a l'état de déplacement de brebis

        //Etat de déplacement de brebis
        if(dogInfos->state == 4){

          int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

          //Si la brebis n'est pas dans l'enclos
          unsigned int distance = sqrt(pow(entityAround[tmpIdSheep].positionX,2)+pow((MAP_SIZE_Y/2) - entityAround[tmpIdSheep].positionY,2));
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
            // Revenir au centre se fera dans l'état 0

            }
          }
    }
}
