typedef struct dog_infos {
  unsigned char position[2];
  unsigned char ID;
  unsigned char nickname[];
} dog;

typedef struct sheep_infos {
  unsigned char position[2];
  unsigned char ID;
} brebis;

int Xmax = 9000;
int Ymax = 6000;
//valeurs modifiables en fonction de la taille de la map

int center_cage[2] = [0; Ymax/2];
