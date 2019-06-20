#ifndef PACKETS
#define PACKETS

unsigned char connectionStart1[] = {255,
                                   00,00,00,00};

unsigned char connectionStart2[] = {254,
                                   13,00,00,00};

int sizeOfConnectionStart1 = 5;
int sizeOfConnectionStart2 = 5;


unsigned char blue1[] = {00,
                        98,108,117,101,49,
                        00};

unsigned char green1[] = {00,
                        103,114,101,101,110,49,
                        00};

unsigned char red1[] = {00,
                       114,101,100,49,
                       00};

unsigned char cyan1[] = {00,
                        99,121,97,110,49,
                        00};

unsigned char purple1[] = {00,
                         112,117,114,112,108,101,49,
                         00};

unsigned char yellow1[] = {00,
                         121,101,108,108,111,119,49,
                         00};


unsigned char *colorPackets1[] = {blue1,green1,red1,cyan1,purple1,yellow1};

char *nicknames1[] = {
    "blue1",
    "green1",
    "red1",
    "cyan1",
    "purple1",
    "yellow1"
};



unsigned char blue2[] = {00,
                        98,108,117,101,50,
                        00};

unsigned char green2[] = {00,
                        103,114,101,101,110,50,
                        00};

unsigned char red2[] = {00,
                       114,101,100,50,
                       00};

unsigned char cyan2[] = {00,
                        99,121,97,110,50,
                        00};

unsigned char purple2[] = {00,
                         112,117,114,112,108,101,50,
                         00};

unsigned char yellow2[] = {00,
                         121,101,108,108,111,119,50,
                         00};

unsigned char *colorPackets2[] = {blue2,green2,red2,cyan2,purple2,yellow2};

char *nicknames2[] = {
    "blue2",
    "green2",
    "red2",
    "cyan2",
    "purple2",
    "yellow2"
};


int sizeOfColorPackets[] = {7,8,6,7,9,9};
int actionRanges[] = {0,250,60,300,100,100};



#endif
