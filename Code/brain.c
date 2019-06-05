#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>

#include "brain.h"
#include "dog.h"

/**************************
\Calcule la strategie (Déplacements aléatoires)
***************************/
void computeStrategy(){

  //Si on a atteint la destination
  if(currentPositionX == targetPositionX && currentPositionY == targetPositionY){

    srandom(time(NULL));

    targetPositionX = (rand()%MAP_SIZE_X);
    targetPositionY = (rand()%MAP_SIZE_Y);
  }

}
