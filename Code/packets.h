#ifndef PACKETS
#define PACKETS

unsigned char connectionStart1[] = {255,
                                   00,00,00,00};

unsigned char connectionStart2[] = {254,
                                   13,00,00,00};

int sizeOfConnectionStart1 = 5;
int sizeOfConnectionStart2 = 5;


unsigned char blue[] = {00,
                        98,108,117,101,49,
                        00};

unsigned char green[] = {00,
                        103,114,101,101,110,49,
                        00};

unsigned char red[] = {00,
                       114,101,100,49,
                       00};

unsigned char cyan[] = {00,
                        99,121,97,110,49,
                        00};

unsigned char purple[] = {00,
                         112,117,114,112,108,101,49,
                         00};

unsigned char yellow[] = {00,
                         121,101,108,108,111,119,49,
                         00};


unsigned char *colorPackets[] = {blue,green,red,cyan,purple,yellow};
int sizeOfColorPackets[] = {7,8,6,7,9,9};
char *nicknames[] = {
    "blue1",
    "green1",
    "red1",
    "cyan1",
    "purple1",
    "yellow1"
};

int actionRanges[] = {0,100,60,300,100,100};



#endif
