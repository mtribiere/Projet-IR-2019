#ifndef DOG_FILE
#define DOG_FILE

#define MAXENTITY 2000
#define MAP_SIZE_X 9000
#define MAP_SIZE_Y 6000

typedef struct entity {
  unsigned int positionX;
  unsigned int positionY;
  unsigned int ID;
} Entity;

unsigned int currentPositionX = 0;
unsigned int currentPositionY = 0;
unsigned int ID;
unsigned char *nickname;

unsigned int targetPositionX = 1000;
unsigned int targetPositionY = 1000;

Entity entityAround[MAXENTITY];
int numberOfEntity = 0;

//valeurs modifiables en fonction de la taille de la map

//int center_cage[] = {0, Ymax/2};

#endif
