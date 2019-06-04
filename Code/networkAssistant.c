#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>


#include "networkAssistant.h"

/**
\Permet de detecter la taille d'un paquet de position
*******************************************/
int getSizeOfPositionPacket(unsigned char *buff,size_t len){
	int sizeToReturn = 0;
	int offset = 0;

	//Tant qu'on a pas atteint la fin du paquet
	while(buff[offset] != 0){

		//Se deplacer avant le nickname
		offset+=17;

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
      offset+=17;

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
\Permet de recuprer le position en X d'une entité
**********************/
