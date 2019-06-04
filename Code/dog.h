typedef struct dog_infos {
  unsigned char position[2];
  unsigned char ID;
  unsigned char nickname[];
} dog;

typedef struct entity {
  unsigned int positionX;
  unsigned int positionY;
  unsigned int ID;
} Entity;

Entity entityAround[MAXENTITY];
int numberOfEntity = 0;

int Xmax = 9000;
int Ymax = 6000;
//valeurs modifiables en fonction de la taille de la map

//int center_cage[] = {0, Ymax/2};
