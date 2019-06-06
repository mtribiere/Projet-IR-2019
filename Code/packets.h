#ifndef PACKETS
#define PACKETS


unsigned char connectionStart1[] = {255,
                                   00,00,00,00};

unsigned char connectionStart2[] = {254,
                                   13,00,00,00};

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

int sizeofBluePacket = 6;
int sizeofGreenPacket = 7;
int sizeofRedPacket = 5;
int sizeofCyanPacket = 6;
int sizeofPurplePacket = 8;
int sizeofYellowPacket = 8;

int sizeogconnectionStart1 = 5;
int sizeogconnectionStart2 = 5;

#endif
