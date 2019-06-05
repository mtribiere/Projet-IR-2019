#include <stdio.h>
#include <stdlib.h>
//#include <getopt.h>
//#include <string.h>
//#include <signal.h>
//#include <syslog.h>
//#include <sys/time.h>
//#include <unistd.h>
#include <math.h>

//#include "test-client.c"
//#include "client.h"

#define LONGUEUR 9000
#define LARGEUR 6000
// A INITIALISER DANS UNE FONCTION AU DEBUT DE LA PARTIE

void print_packet(unsigned char* packet);
void print_position(unsigned int* position);




// CHANGER TOUS LES INT EN UNSIGNED INT

//On prend en argument trois nombres dont deux non limités à 256
//On veut retourner tableau contenant le premier nombre et la décomposition en hexadécimal des deux autres nombres
unsigned char* packet_creator(unsigned int x_position,unsigned int y_position)
{
	unsigned char* packet=malloc(8*sizeof(unsigned char));

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
        else if (y_position>256 && y_position<(256*256))
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

	unsigned char tab[]={x1,x2,x3,x4,y1,y2,y3,y4};
	for (i=0;i<8;i++)
	{
		packet[i]=tab[i];
	}

	return packet;
}

unsigned char* packet_creator_completed(unsigned char* packet)
{
        unsigned char* packet_completed=malloc(13*sizeof(unsigned char));
	int i;

	packet_completed[0]=16;
	for (i=1;i<9;i++)
        {
                packet_completed[i]=packet[i-1];
        }
	for (i=9;i<13;i++)
	{
		packet_completed[i]=00;
	}
  			return packet_completed;
}


//FAIRE UNE FONCTION POUR TRANSFORMER TOUS LES Xi ET Yi EN HEXADECIMAL => PAQUET PRET A L ENVOI



//On prend en argument un tableau contenant un premier nombre et la décomposition en héxadécimal de deux nombres
//On veut retourner un tableau de trois nombres dont deux non limités à 256
unsigned int* position_creator(unsigned char* packet)
{
        unsigned int* position=malloc(2*sizeof(unsigned int));

	unsigned int x_position,y_position;
	int i;

        // Calcul des  positions de l'objet :
	x_position=packet[0];
	if(packet[1]!=0)
	{
		x_position+=256*packet[1];
	}
	if(packet[2]!=0)
	{
		x_position+=256*256*packet[2];
	}

	y_position=packet[4];
        if(packet[5]!=0)
        {
                y_position+=256*packet[5];
        }
        if(packet[6]!=0)
        {
                y_position+=256*256*packet[6];
        }

       	unsigned int tab[]={x_position,y_position};
        for (i=0;i<2;i++)
        {
                position[i]=tab[i];
        }

        return position;
}


void print_packet(unsigned char* packet)
{
        int i;
        for (i=0;i<4;i++)
        {
                printf("%d ",packet[i]);
        }
        printf("\n");
        for (i=4;i<8;i++)
        {
                printf("%d ",packet[i]);
        }
        printf("\n");
}


void print_position(unsigned int* position)
{
	printf("%d %d\n",position[1],position[2]);
}


//Le serveur nous envoie un paquet qui contient notre position et les positions des brebis qu'on voit
//Nous lui renvoyons un paquet qui contient la position de la souris = position de destination


int Arrondir(float reel)
{
        float nombre_de_depart=reel;
        int reel_arrondi,reste;
        while(reel>1)
        {
                reel=reel-1;
        }
        while(reel<0)
        {
                reel=reel+1;
        }

        if (reel>=0.5)
        {
                reste=reel+1;
        }
        else
        {
                reste=reel;
        }
        reel_arrondi=nombre_de_depart-reel+reste;
	return reel_arrondi;
}



//On prend en argument un rayon d'action et deux positions (celle du chien actuellement et celle d'une brebis)
//On veut renvoyer une position (celle à laquelle le chien doit se rendre pour éviter la brebis PUIS se retrouver derrière elle, prêt à la pousser)
//On fait un position_calculator_aux qui évite la brebis puis un position_calculator qui se place derrière elle
unsigned int* position_calculator_aux_1(int rayon, unsigned int* position_brebis)
{
	unsigned int x_position_aimed, y_position_aimed;
	unsigned int* position_aimed=malloc(2*sizeof(unsigned int));
	int i;

	//Stratégie de base
	if(position_brebis[2]<=LARGEUR/2)
	{
		x_position_aimed=position_brebis[1];
		y_position_aimed=position_brebis[2]+(rayon+1);
	}
	else if(position_brebis[2]>LARGEUR/2)
        {
                x_position_aimed=position_brebis[1];
                y_position_aimed=position_brebis[2]-(rayon+1);
        }

	unsigned int tab[]={x_position_aimed,y_position_aimed};
        for (i=0;i<2;i++)
        {
                position_aimed[i]=tab[i];
        }

        return position_aimed;
}


unsigned int* position_calculator_aux_2(int rayon, unsigned int* position_brebis)
{
        unsigned int x_position_aimed, y_position_aimed;
        unsigned int* position_aimed=malloc(2*sizeof(unsigned int));
	int i;

        //Stratégie de base
        if(position_brebis[2]<=LARGEUR/2)
        {
                x_position_aimed=position_brebis[1]+(rayon+1);
                y_position_aimed=position_brebis[2]+(rayon+1);
        }
        else if(position_brebis[2]>LARGEUR/2)
        {
                x_position_aimed=position_brebis[1]+(rayon+1);
                y_position_aimed=position_brebis[2]-(rayon+1);
        }

        unsigned int tab[]={x_position_aimed,y_position_aimed};
        for (i=0;i<2;i++)
        {
                position_aimed[i]=tab[i];
        }

        return position_aimed;
}


unsigned int* position_calculator_aux_3(int rayon, unsigned int* position_brebis)
{
        unsigned int x_position_aimed, y_position_aimed;
        unsigned int* position_aimed=malloc(2*sizeof(unsigned int));
	int i;

        //Stratégie de base
        if(position_brebis[2]<=LARGEUR/2)
        {
                x_position_aimed=position_brebis[1]+(rayon+1);
                y_position_aimed=position_brebis[2]-(rayon+1);
        }
        else if(position_brebis[2]>LARGEUR/2)
        {
                x_position_aimed=position_brebis[1]+(rayon+1);
                y_position_aimed=position_brebis[2]+(rayon+1);
        }

        unsigned int tab[]={x_position_aimed,y_position_aimed};
        for (i=0;i<2;i++)
        {
                position_aimed[i]=tab[i];
        }

        return position_aimed;
}


unsigned int* position_calculator(int rayon, unsigned int* position_brebis)
{
        unsigned int x_position_aimed, y_position_aimed;
	float coefficient;
        unsigned int* position_aimed=malloc(2*sizeof(unsigned int));
	int i;

	coefficient=position_brebis[2]/position_brebis[1];

        // Deux équations : Yfinal = coefficient*Xfinal
        // Deux inconnues : sqrt(Xfinal² + Yfinal²) = rayon+1
        // On résoud le système et on trouve :
        // X = sqrt((rayon+1)/(1+coefficient²))
        // Y = coefficent*sqrt((rayon+1)/(1+coefficient²))


        //Stratégie de base
        if(position_brebis[2]<=LARGEUR/2)
        {
                x_position_aimed=position_brebis[1]+sqrt((rayon+1)/(1+coefficient*coefficient));
                y_position_aimed=position_brebis[2]-coefficient*sqrt((rayon+1)/(1+coefficient*coefficient));
        }
        else if(position_brebis[2]>LARGEUR/2)
        {
                x_position_aimed=position_brebis[1]+sqrt((rayon+1)/(1+coefficient*coefficient));
                y_position_aimed=position_brebis[2]+coefficient*sqrt((rayon+1)/(1+coefficient*coefficient));
        }

	// Stratégie améliorée à modifier
	/*
        if(position_brebis[1]<=Arrondir(LONGUEUR/3))
        {
                Stratégie de base
        }
        else if(position_brebis[1]>Arrondir(LONGUEUR/3))
        {
                Définir positions du point O puis modifier la suite
                if(position_brebis[1]<=LARGEUR/2)
                {
                        x_position_aimed=position_brebis[1];
                        y_position_aimed=position_brebis[2]+(rayon+1);
                }
                else if(position_brebis[1]>LARGEUR/2)
                {
                        x_position_aimed=position_brebis[1];
                        y_position_aimed=position_brebis[2]-(rayon+1);
                }
        }
        */


        unsigned int tab[]={x_position_aimed,y_position_aimed};
        for (i=0;i<2;i++)
        {
                position_aimed[i]=tab[i];
        }

        return position_aimed;
}


/*
int main(void)
{
	unsigned char* paquet1;
	unsigned char* paquet2;

	unsigned int* position1;
	unsigned int* position2;

	paquet1=packet_creator(62,62);
	paquet2=packet_creator(6238,1271);
	printf("\n\n\n\n");
	print_packet(paquet1);
	print_packet(paquet2);

	paquet1[0]=10;
	paquet1[1]=1;
	paquet1[2]=0;
	paquet1[3]=0;

        paquet2[0]=10;
        paquet2[1]=1;
        paquet2[2]=1;
        paquet2[3]=0;

	position1=position_creator(paquet1);
	position2=position_creator(paquet2);
	printf("\n\n\n\n");
	print_position(position1);
	print_position(position2);

	printf("\n");
	return 1;
}

*/
