#ifndef NETWORK_ASSISTANT
#define NETWORK_ASSISTANT

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>

int getSizeFromPositionPacket(unsigned char *buff,size_t len);
int getIDFromPositionPacket(unsigned char *buff,size_t len,int id);
unsigned int *getPosFromPositionPacket(unsigned char *buff,size_t len,int id);
char *getNicknameFromPositionPacket(unsigned char *buff,size_t len,int id);

#endif
