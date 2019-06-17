#ifndef ENTITY_FILE
#define ENTITY_FILE

#define MAXENTITY 2000
#define ENTITY_SIZE 38
#define POSITION_MARGIN 2
#define PUSHING_MARGIN 20

typedef struct entity {
  unsigned int positionX;
  unsigned int positionY;
  unsigned int ID;
  char *nickname;
} Entity;

typedef struct dog{
  Entity entity;
  int state;
  int targetedSheepId;
  int dogType;
  int actionRange;
  time_t timer;
  unsigned int targetPositionX;
  unsigned int targetPositionY;
}Dog;

#endif
