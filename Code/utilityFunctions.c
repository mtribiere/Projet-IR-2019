#include <stdio.h>
#include <stdlib.h>

void print_packet(unsigned char* packet);
void print_position(int* position);


//On prend en argument trois nombres dont deux non limités à 256
//On veut retourner tableau contenant le premier nombre et la décomposition en hexadécimal des deux autres nombres
unsigned char* packet_creator(unsigned char packet_number,int x_position,int y_position)
{
	unsigned char* packet=malloc(9*sizeof(unsigned char));

	int i;

	unsigned char x1,x2,x3,x4;
	unsigned char y1,y2,y3,y4;

	// Calcul des nombres à rentrer pour atteindre les positions souhaitees :
	if (x_position<256)
	{
		x3=00;
                x2=00;
		x1=x_position;
	}
	else if (x_position>256 && x_position<(256*256))
	{
		x3=00;
		x2=x_position/256;
		x1=x_position-x2*256;
	}
        else if (x_position>(256*256))
	{
		x3=x_position/(256*256);
		x2=(x_position-x3*256*256)/256;
		x1=(x_position-x3*256*256-x2*256);
	}
        x4=00;

	if (y_position<256)
        {
		y3=00;
		y2=00;
                y1=y_position;
        }
        else if (y_position>256)
        {
		y3=00;
                y2=y_position/256;
                y1=y_position-y2*256;
        }
        else if (y_position>(256*256))
        {
                y3=y_position/(256*256);
                y2=(y_position-y3*256*256)/256;
                y1=(y_position-y3*256*256-y2*256);
        }
        y4=00;

	unsigned char tab[]={packet_number,x1,x2,x3,x4,y1,y2,y3,y4};
	for (i=0;i<9;i++)
	{
		packet[i]=tab[i];
	}

	return packet;
}


//On prend en argument un tableau contenant un premier nombre et la décomposition en héxadécimal de deux nombres
//On veut retourner un tableau de trois nombres dont deux non limités à 256
int* position_creator(unsigned char* packet)
{
        int* position=malloc(3*sizeof(unsigned char));

	int packet_number;
	int x_position,y_position;
	int i;

	packet_number=packet[0];

        // Calcul des  positions de l'objet :
	x_position=packet[1];

	if(packet[2]!=0)
	{
		x_position+=256*packet[2];
	}
	if(packet[3]!=0)
	{
		x_position+=256*256*packet[3];
	}

	y_position=packet[5];
        if(packet[6]!=0)
        {
                y_position+=256*packet[6];
        }
        if(packet[7]!=0)
        {
                y_position+=256*256*packet[7];
        }

       	int tab[]={packet_number,x_position,y_position};
        for (i=0;i<3;i++)
        {
                position[i]=tab[i];
        }

        return position;
}



void print_packet(unsigned char* packet)
{
	int i=0;
	printf("%d\n",packet[0]);
	for (i=1;i<5;i++)
	{
		printf("%d ",packet[i]);
	}
	printf("\n");
	for (i=5;i<9;i++)
        {
                printf("%d ",packet[i]);
        }
	printf("\n");
}


void print_position(int* position)
{
	printf("%d %d %d\n",position[0],position[1],position[2]);
}
