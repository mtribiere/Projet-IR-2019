#ifndef PACKETS
#define PACKETS

unsigned char connectionStart1[] = {255,
                                   00,00,00,00};

unsigned char connectionStart2[] = {254,
                                   13,00,00,00};

int sizeOfConnectionStart1 = 5;
int sizeOfConnectionStart2 = 5;


unsigned char blue[] = {00,
                        98,108,117,101,
                        00};

unsigned char green[] = {00,
                        103,114,101,101,110,
                        00};

unsigned char red[] = {00,
                       114,101,100,
                       00};

unsigned char cyan[] = {00,
                        99,121,97,110,
                        00};

unsigned char purple[] = {00,
                         112,117,114,112,108,101,
                         00};

unsigned char yellow[] = {00,
                         121,101,108,108,111,119,
                         00};


unsigned char *colorPackets[] = {blue,green,red,cyan,purple,yellow};
int sizeOfColorPackets[] = {6,7,5,6,8,8};
char *nicknames[] = {
    "blue",
    "green",
    "red",
    "cyan",
    "purple",
    "yellow"
};

int actionRanges[] = {0,300,60,300,100,100};



#endif
