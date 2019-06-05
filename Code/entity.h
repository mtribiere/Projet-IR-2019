#ifndef ENTITY_FILE
#define ENTITY_FILE

#define MAXENTITY 2000

typedef struct entity {
  unsigned int positionX;
  unsigned int positionY;
  unsigned int ID;
  unsigned char *nickname;
} Entity;

typedef struct dog{
  Entity entity;
  unsigned int targetPositionX;
  unsigned int targetPositionY;
}Dog;

#endif
