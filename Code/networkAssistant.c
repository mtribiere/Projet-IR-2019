#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>


#include "networkAssistant.h"
#include "utilityFunctions.h"

/**
\Permet de detecter la taille d'un paquet de position
*******************************************/
int getSizeFromPositionPacket(unsigned char *buff,size_t len){
	int sizeToReturn = 0;
	int offset = 0;

	//Tant qu'on a pas atteint la fin du paquet
	while(buff[offset] != 0){

		//Se deplacer avant le nickname
		offset+=18;

		//Se deplacer après le nickname
		while(buff[offset] != 0) offset++;
		offset++;
		sizeToReturn++;

	 }

	return sizeToReturn;
}


/*******************************
\Permet de recuperer l'ID d'une entité dans un packet (16)
*******************************/
int getIDFromPositionPacket(unsigned char *buff,size_t len,int id){
    int offset = 0;
    int counter = 0;
    int toReturn;

    //Tant qu'on a pas atteint la fin du paquet
    while(buff[offset] != 0 && counter != id){
      //printf("Check packet %d\n",counter);

      //Se deplacer avant le nickname
      offset+=18;
      //Se deplacer après le nickname (le paquet suivant)
      while(buff[offset] != 0) offset++;
      offset++;
      counter++;
    }

    //Prendre l'id
    toReturn = buff[offset];

    return toReturn;
}



/*********************
\Permet de recuprer la position d'une entité dans un packet (16)
**********************/

unsigned int* getPosFromPositionPacket(unsigned char *buff,size_t len,int id){
		int offset = 0;
		int counter = 0;

		//Tant qu'on a pas atteint la fin du paquet
		while(buff[offset] != 0 && counter != id){

			//Se deplacer avant le nickname
			offset+=18;

			//Se deplacer après le nickname (le paquet suivant)
			while(buff[offset] != 0) offset++;
			offset++;
			counter++;

		}

		//Prendre la position
		unsigned char pos[] = {buff[offset+4],buff[offset+5],buff[offset+6],buff[offset+7],buff[offset+8],buff[offset+9],buff[offset+10],buff[offset+11]};

		//Retourner la valeur converti
		return position_creator(pos);

}


/*********************
\Permet de recuprer le nickname d'une entité dans un packet (16)
**********************/

char *getNicknameFromPositionPacket(unsigned char *buff,size_t len,int id){
	int offset = 0;
	int counter = 0;
	char *toReturn;

	//Tant qu'on a pas atteint la fin du paquet
	while(buff[offset] != 0 && counter != id){

		//Se deplacer avant le nickname
		offset+=18;

		//Si ce n'est pas le nickname qu'on cherche
		if(counter != id){

			//Se deplacer après le nickname (le paquet suivant)
			while(buff[offset] != 0) offset++;
			offset++;
			counter++;
		}

	}

	//Se deplacer avant le nickname
	offset+=18;
	
	//Recuperer la taille du nickname
	int sizeOfNickname = 0;
	while(buff[offset+sizeOfNickname] != 0) sizeOfNickname++;

	//Creer et recuperer le nickname
	toReturn = malloc(sizeof(char)*sizeOfNickname+1);
	for(int i = 0;i<sizeOfNickname;i++){
		toReturn[i] = buff[offset+i];
	}
	toReturn[sizeOfNickname] = '\0';


	return ((char *)toReturn);
}
