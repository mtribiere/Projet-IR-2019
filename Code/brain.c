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
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
      }
    }else{ // Si on est en chasse
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
        dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
        dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
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
              printf("\n\n Condition non vérifiée : \nDelta X carré = %f \nPosition en Y : %d \nMAP_SIZE_Y / 2 = %d \nDelta Y = %d \nDelta Y carré = %f \nSomme = %f \nDistance = %f \n\n",pow((dogInfos->entity).positionX,2),(dogInfos->entity).positionY,MAP_SIZE_Y/2,abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2),pow((dogInfos->entity).positionX,2)+pow(abs(MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2),sqrt(pow((dogInfos->entity).positionX,2) + pow((MAP_SIZE_Y/2 - (dogInfos->entity).positionY),2)));
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
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }

        else{
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
          dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
          dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
        }
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
          if(entityAround[i].positionX < ((MAP_SIZE_X/10)-PUSHING_MARGIN) || entityAround[i].positionY > ((MAP_SIZE_Y/2)+(MAP_SIZE_X/10)-PUSHING_MARGIN) || entityAround[i].positionY < ((MAP_SIZE_Y/2)-(MAP_SIZE_X/10)+PUSHING_MARGIN)){
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

        if(isEntityInBase(entityAround[tmpIdSheep])==0){
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
          }else{ // La brebis est dans l'enclos
            dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
            dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
            if(isTargetPositionReached(dogInfos)) dogInfos->state=0;
          }

      //Si on est aligné
      if(dogInfos->state == 2) dogInfos->state = 3; // On passe a l'état de déplacement du chien à proximité du centre

      //Etat de déplacement du chien à proximité du centre
      if(dogInfos->state == 3){

        int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);
        int angleRatio;
        //unsigned int distance;

        if(entityAround[tmpIdSheep].positionX == MAP_SIZE_X/2){
          angleRatio = 1;
        }else{
          angleRatio = (MAP_SIZE_Y/2 - entityAround[tmpIdSheep].positionY) / (MAP_SIZE_X/2 - entityAround[tmpIdSheep].positionX);
        }
        /*
        distance= sqrt(pow(MAP_SIZE_Y/2 - entityAround[tmpIdSheep].positionY,2) + pow(MAP_SIZE_X/2 - entityAround[tmpIdSheep].positionX,2));
        distance = distance - 400; // On retire deux tiers du champ de vision d'un jaune pour avoir la distance à parcourir pour être vu sans être au centre
        printf("\n\n On a un ratio de %d pour une distance au centre de %d (avec une distance initiale de %d) \n\n",angleRatio,distance,distance+400);
        */
        printf("\n\n On a un ratio de %d \n\n",angleRatio);

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

        printf("\n\n On veut une coordonnée en x de %d \n",dogInfos->targetPositionX);
        printf(" On veut une coordonnée en y de %d \n\n",dogInfos->targetPositionY);

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

        printf("\n\n On veut une coordonnée en x de %d \n",dogInfos->targetPositionX);
        printf(" On veut une coordonnée en y de %d \n\n",dogInfos->targetPositionY);
      }

      if(isTargetPositionReached(dogInfos)) dogInfos->state = 5;
      else printf("\n\n Coordonnées encore non atteintes \n\n");
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

      printf("Fin de la passation \n\n\n\n\n\n");
    }
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

      // Quel que soit l'état, si on a rencontré un problème
      // On revient vers le centre
      if( (dogInfos->targetPositionX > (MAP_SIZE_X - dogInfos->actionRange)) || (dogInfos->targetPositionY > (MAP_SIZE_Y - dogInfos->actionRange)) || (dogInfos->targetPositionX < dogInfos->actionRange) || (dogInfos->targetPositionY < dogInfos->actionRange) ){
        dogInfos->targetPositionX = MAP_SIZE_X/2;
        dogInfos->targetPositionY = MAP_SIZE_Y/2;
      }

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

        printf("\n\n Passage à l'état 1 \n\n");

        int isBrebisNear = 0;
        // Se réinitialise lorsqu'on revient de l'état > 1

        //Chercher quel jaune va bouger et de combien
        for(int idAround = 0;idAround<numberOfEntity;idAround++){
          //Si c'est un chien bleu, on retient sa directon
          if(entityAround[idAround].nickname[0] == 'b' && entityAround[idAround].nickname[1] == 'l' && entityAround[idAround].nickname[2] == 'u' && entityAround[idAround].nickname[3]=='e'){
            if(deplacement_sur_x == 0 && deplacement_sur_y == 0){ // Si le jaune ne connait pas déjà une valeur de destination
              deplacement_sur_x = entityAround[idAround].positionX - MAP_SIZE_X/2;
              deplacement_sur_y = entityAround[idAround].positionY - MAP_SIZE_Y/2;
            }
            printf("\n\n MODIFICATION DES PARAMETRES \n\n");
          }
          // Sinon si c'est un chien jaune, on choisit le jaune à déplacer
          if (entityAround[idAround].nickname[0] == 'y' && entityAround[idAround].nickname[1] == 'e' && entityAround[idAround].nickname[2] == 'l' && entityAround[idAround].nickname[3]=='l' && entityAround[idAround].nickname[4] == 'o' && entityAround[idAround].nickname[5]=='w'){
            idYellowToMove = min(entityAround[idAround].ID,(dogInfos->entity.ID));
          }
          if (entityAround[idAround].nickname[0] == 'b' && entityAround[idAround].nickname[1] == 'o' && entityAround[idAround].nickname[2] == 't'){
            isBrebisNear++;
          }
        }
        printf("\n\n Paramètres : \n deplacement_sur_x = %d\n deplacement_sur_y = %d\n idYellowToMove = %d\n isBrebisNear = %d\n \n\n",deplacement_sur_x,deplacement_sur_y,idYellowToMove,isBrebisNear);



        if((deplacement_sur_x != 0 || deplacement_sur_y != 0) && (dogInfos->entity).ID == idYellowToMove){
            if(isTargetPositionReached(dogInfos)){
                printf("\n\n BLEU DETECTE. ON MODIFIE LES POSITIONS DE DESTINATION. \n\n");
                // Il s'agit ici de calculer k tel que deplacement_sur_x*k < MAP_SIZE_X && deplacement_sur_y*k < MAP_SIZE_Y && k soit le plus grand possible
                /*
                int facteur_max = 0;
                while(deplacement_sur_x*facteur_max < MAP_SIZE_X && deplacement_sur_y*facteur_max < MAP_SIZE_Y){
                  facteur_max++;
                }
                // On diminue facteur_max de 1 pour que les deux conditions précédentes soient à nouveau vérifiées
                facteur_max--;
                dogInfos->targetPositionX += deplacement_sur_x*facteur_max;
                dogInfos->targetPositionY += deplacement_sur_y*facteur_max;
                printf("\n\n Facteur max : %d\n deplacement_sur_x : %d\n deplacement_sur_y : %d\n",facteur_max,deplacement_sur_x,deplacement_sur_y);*/
                dogInfos->targetPositionX += deplacement_sur_x;
                dogInfos->targetPositionY += deplacement_sur_y;
                printf("\n Mise en mouvement :\n Cible en x : %d\n Cible en y : %d\n \n\n",dogInfos->targetPositionX,dogInfos->targetPositionY);

            }
          }
          if (isBrebisNear != 0){  // Il y a au moins une brebis à proximité à ramener à l'enclos
              dogInfos->state = 2;
              printf("\n\n Passage à l'état 2 \n\n");

              int tmpIdSheep = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);
              dogInfos->targetedSheepId = tmpIdSheep;

              // UNE BREBIS EST DANS L'ENCLOS SSI :
              // entityAround[i].positionX >= (MAP_SIZE_X/10)-PUSHING_MARGIN
              // ET (entityAround[i].positionY <= (MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+PUSHING_MARGIN)
              // ET (entityAround[i].positionY >= (MAP_SIZE_Y/2)-(MAP_SIZE_X/10)-PUSHING_MARGIN)

              //Si la brebis n'est pas dans l'enclos
              //unsigned int distance = sqrt(pow(entityAround[tmpIdSheep].positionX,2)+pow((MAP_SIZE_Y/2) - entityAround[tmpIdSheep].positionY,2));
              //printf("\n\n Distance : %d \n\n",distance);

              // Si le chien qui pousse la brebis n'est pas dans l'enclos
              if(isEntityInBase(dogInfos->entity)==0){

                  //Continuer de pousser la brebis en réactualisant les paramètres
                  int angleRatio = entityAround[tmpIdSheep].positionY / entityAround[tmpIdSheep].positionX;
                  printf("\n\n angleRatio : %d \n\n",angleRatio);

                  dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX + sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));

                  //Si on est dans le haut de la map
                  if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
                    dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY - angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
                  }else{
                    dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY + angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
                  }
                  printf("\n\n Modification du mouvement :\n Cible en x : %d\n Cible en y : %d\n \n\n",dogInfos->targetPositionX,dogInfos->targetPositionY);
              }else{ // Chien qui pousse la brebis dans l'enclos

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

          //Si le chien qui pousse la brebis n'est pas dans l'enclos
          //unsigned int distance = sqrt(pow(entityAround[tmpIdSheep].positionX,2)+pow((MAP_SIZE_Y/2) - entityAround[tmpIdSheep].positionY,2));
          if(isEntityInBase(entityAround[tmpIdSheep])==0){

              //Continuer de pousser la brebis en réactualisant les paramètres
              int angleRatio = entityAround[tmpIdSheep].positionY / entityAround[tmpIdSheep].positionX;

              dogInfos->targetPositionX = entityAround[tmpIdSheep].positionX + sqrt(dogInfos->actionRange / (1+pow(angleRatio,2)));

              //Si on est dans le haut de la map
              if((dogInfos->entity).positionY <= MAP_SIZE_Y/2){
                dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY - angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
              }else{
                dogInfos->targetPositionY = entityAround[tmpIdSheep].positionY + angleRatio*sqrt(dogInfos->actionRange / (1 + pow(angleRatio,2)));
              }

          }else{ // Chien qui pousse la brebis dans l'enclos

            //Arreter la chasse
            dogInfos->state = 0;
            dogInfos->targetedSheepId = 0;

            // Revenir au centre
            dogInfos->targetPositionX = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_X-ENTITY_SIZE);
            dogInfos->targetPositionY = generateRandomPosition(ENTITY_SIZE,MAP_SIZE_Y-ENTITY_SIZE);
            }
          }
    }

    //Si on est un chien Vert (Simple carré haut ou bas)
    if(dogInfos->dogType == 1){

      //Si un partenaire n'a pas été trouvé
      if(dogInfos->targetedSheepId == 0){

        //Aller au meetPoint
        if(BASE_SIDE == 1) dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
        if(BASE_SIDE == 2) dogInfos->targetPositionX = ENTITY_SIZE;
        dogInfos->targetPositionY = (MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+100;
      }

      //Etat initial (attente de trouver le partenaire)
      if(dogInfos->state == 0){

        //Detecter si il y a un partenaire
        for(int i = 0;i<numberOfEntity;i++){
          //Si c'est un partenaire
          if((strcmp(entityAround[i].nickname,"green1") == 0 && BASE_SIDE == 1) || (strcmp(entityAround[i].nickname,"green2") == 0 && BASE_SIDE == 2)){
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
        if(isTargetPositionReached(dogInfos)){
          printf("Waiting parterner positionned\n");

            //Si le partenaire est en place
            int tmpId = findIdOfSheep(entityAround,numberOfEntity,dogInfos->targetedSheepId);

            //Si pas d'erreur de synchronisation
            if(tmpId != -1){
              //En X
              if((entityAround[tmpId].positionX >= MAP_SIZE_X-ENTITY_SIZE-POSITION_MARGIN && entityAround[tmpId].positionX <= MAP_SIZE_X-ENTITY_SIZE+POSITION_MARGIN) || (entityAround[tmpId].positionX >= ENTITY_SIZE-POSITION_MARGIN && entityAround[tmpId].positionX <= ENTITY_SIZE+POSITION_MARGIN)){
                //En Y
                if(entityAround[tmpId].positionY <= (MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+100+POSITION_MARGIN && entityAround[tmpId].positionY >= (MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+100-POSITION_MARGIN){
                  printf("Waiting sheeps\n");
                  int sheepInFront = 0;
                  //Pour toutes les entités
                  for(int i = 0;i<numberOfEntity;i++){
                    //Si on voit une brebis
                    if(strstr(entityAround[i].nickname,"bot") != NULL){
                      //Si elle est devant nous
                      if(entityAround[i].positionY <= (dogInfos->entity).positionY+PUSHING_MARGIN && entityAround[i].positionY >= (dogInfos->entity).positionY-PUSHING_MARGIN){
                        sheepInFront = 1;
                      }
                    }
                  }

                  //Si une brebis est devant nous
                  if(sheepInFront){
                    //Commencer le pattern
                    (dogInfos->state)++;

                    //Ne plus bouger
                    dogInfos->targetPositionX = (dogInfos->entity).positionX;
                    dogInfos->targetPositionY = (dogInfos->entity).positionY;
                  }
                }
              }
            }else{//Erreur de synchronisation
              dogInfos->state = 0;
              dogInfos->targetedSheepId = 0;
            }
          }
      }
      ////////////////Point 2 / Spécial
      if(dogInfos->state == 4 || dogInfos->state == 14){//Si on est celui du dessus
        //Si on a atteint la position X voulu
        if(((dogInfos->entity).positionX <= ENTITY_SIZE && BASE_SIDE == 1) || ((dogInfos->entity).positionX >= MAP_SIZE_X-ENTITY_SIZE && BASE_SIDE == 2)){
          //Ne plus bouger et passer au point suivant
          dogInfos->targetPositionX = (dogInfos->entity).positionX;
          dogInfos->targetPositionY = (dogInfos->entity).positionY;

          //Passer à l'etat suivant
          (dogInfos->state)++;

        }else{//Si on a pas atteint la position
          if(BASE_SIDE == 1) dogInfos->targetPositionX = (dogInfos->entity).positionX - PUSHING_FORCE;//Vitesse de déplacment
          if(BASE_SIDE == 2) dogInfos->targetPositionX = (dogInfos->entity).positionX + PUSHING_FORCE;
          dogInfos->targetPositionY = (dogInfos->entity).positionY;
        }
      }


      if(dogInfos->state != 0 && dogInfos->state != 1 && dogInfos->state != 11 && dogInfos->state != 4 && dogInfos->state != 14 && isTargetPositionReached(dogInfos)){
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
          if(BASE_SIDE == 1) dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
          if(BASE_SIDE == 2) dogInfos->targetPositionX = ENTITY_SIZE;
          dogInfos->targetPositionY = (MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+100-(dogInfos->actionRange);
        }
        if(dogInfos->state == 13){//Si on est celui du dessous
          if(BASE_SIDE == 1) dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
          if(BASE_SIDE == 2) dogInfos->targetPositionX = ENTITY_SIZE;
          dogInfos->targetPositionY = (MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+100+(dogInfos->actionRange);
        }

        //////////////Point 3
        if(dogInfos->state == 6 || dogInfos->state == 16){//Si on est celui du dessus
          if(BASE_SIDE == 1) dogInfos->targetPositionX = ENTITY_SIZE;
          if(BASE_SIDE == 2) dogInfos->targetPositionX = MAP_SIZE_X-ENTITY_SIZE;
          dogInfos->targetPositionY = (MAP_SIZE_Y/2)+(MAP_SIZE_X/10)+100-(dogInfos->actionRange)-300;
        }


        ////////////Point 4
        if(dogInfos->state == 7){//Si on est celui du dessus
          dogInfos->targetPositionX = (MAP_SIZE_X/2)-300;
          dogInfos->targetPositionY = ENTITY_SIZE;
        }
        if(dogInfos->state == 17){//Si on est celui du dessous
          dogInfos->targetPositionX = (MAP_SIZE_X/2)-300;
          dogInfos->targetPositionY = MAP_SIZE_Y-ENTITY_SIZE;
        }
      }
  }

  //Pour la strategie en duel
  if(dogInfos->dogType == 2 || dogInfos->dogType == 3 || dogInfos->dogType == 5){
    computeStrategyDuel(dogInfos,entityAround,numberOfEntity);
  }

}
