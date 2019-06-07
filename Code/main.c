#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>

#include "client.h"
#include "packets.h"
#include "dog.h"
#include "entity.h"
#include "brain.h"
#include "networkAssistant.h"
#include "utilityFunctions.h"

// compile with gcc -Wall -g -o sock ./test-client.c -lwebsockets


// =====================================================================================================================================
//	Start of function definition
// =====================================================================================================================================

// Caught on CTRL C
void sighandler(int sig)
{
	forceExit = 1;
}

/**
\brief Allocate a packet structure and initialise it.
\param none
\return pointer to new allocated packet
****************************************************************************************************************************/
t_packet *allocatePacket()
{
	t_packet *tmp;

	if ((tmp=malloc(sizeof(t_packet))) == NULL ) return NULL;
	memset(tmp,0,sizeof(t_packet));
	return tmp;
}

/**
\brief Add a packet to the list of packet to be sent
\param wsi websocket descriptor
\param buf buffer to be sent
\param len length of packet
\return pointer to new allocated packet
****************************************************************************************************************************/
int sendCommand(struct lws *wsi,unsigned char *buf,unsigned int len)
{
	t_packet *tmp,*list=packetList;

	if (len > MAXLEN ) return -1;
	if ((tmp=allocatePacket()) == NULL ) return -1;
	memcpy(&(tmp->buf)[LWS_PRE],buf,len);
	tmp->len=len;
	if (packetList == NULL )
		packetList=tmp;
	else {
		while (list && list->next) {
			list=list->next;
		}
		list->next=tmp;
	}
	lws_callback_on_writable(wsi);
	return 1;
}

/**
\Recevoir un message
********************************************************/
int messageReceived(struct lws *wsi,unsigned char *buff,size_t len){

		//Si le message recu est une mise a jour de position
		if(buff[0] == 16){
			/////////////Supprimer les anciens nickname (fuites de memoire)
			for(int i = 0;i<numberOfEntity;i++) free(entityAround[i].nickname);

			////////////Mettre a jour les entité presentes
				//Definir le nombre d'entité aux alentours
				numberOfEntity = getSizeFromPositionPacket(buff+3,len)-1;
				printf("Nombre d'entité : %d\n",numberOfEntity);

				//Récuper leurs infos et les convertir en struct
				for(int i = 0;i<numberOfEntity;i++){
					//Declarer le struct
					Entity tmp;

					//Recuperer l'ID
					tmp.ID = getIDFromPositionPacket(buff+3,len,i);

					//Recuperer la position
					unsigned int *pos = getPosFromPositionPacket(buff+3,len,i);
					tmp.positionX = pos[0];
					tmp.positionY = pos[1];

					//Recuperer le nickname
					tmp.nickname = getNicknameFromPositionPacket(buff+3,len,i);

					//Ajouter l'entité au tableau
					entityAround[i] = tmp;

					//DEBUG
					printf("Entite %s ; ID : %d ; Position = (%d;%d) \n",entityAround[i].nickname,entityAround[i].ID,entityAround[i].positionX,entityAround[i].positionY);

				}

				////////////Chercher notre position dans le packet
				int idInPositionPacket = 0;
				while(dogInfos.entity.ID != getIDFromPositionPacket(buff+3,len,idInPositionPacket) && idInPositionPacket<=numberOfEntity) idInPositionPacket++;

				////////////Mettre a jour notre position
				unsigned int *posCurrent = getPosFromPositionPacket(buff+3,len,idInPositionPacket);
				dogInfos.entity.positionX = posCurrent[0];
				dogInfos.entity.positionY = posCurrent[1];

				printf("Dog %d Position : (%d;%d)\n",dogInfos.entity.ID,dogInfos.entity.positionX,dogInfos.entity.positionY);
				printf("Dog %d Target :   (%d;%d)\n",dogInfos.entity.ID,dogInfos.targetPositionX,dogInfos.targetPositionY);

				///////////Calculer la strategie
				computeStrategy(&dogInfos,entityAround,numberOfEntity);

				///////////Envoyer un packet de position
				//Si on doit bouger
				if(dogInfos.targetPositionX != 0 && dogInfos.targetPositionY != 0){

					unsigned char* toSendPacket = packet_creator(dogInfos.targetPositionX,dogInfos.targetPositionY);
					sendCommand(wsi,toSendPacket,13);

				}

				printf("\n\n");
		}

		//Si le message recu est l'attribution d'ID
		if(buff[0] == 32){
			//Si notre ID n'est pas déja defini
			if(dogInfos.entity.ID == 0){
				dogInfos.entity.ID = buff[1];
			}
		}

	return 0;
}



/****************************************************************************************************************************/
int writePacket(struct lws *wsi)
{
	t_packet *tmp=packetList;
	int ret;

	if (packetList == NULL ) return 0;

	packetList=tmp->next;
	ret=lws_write(wsi,&(tmp->buf)[LWS_PRE],tmp->len,LWS_WRITE_BINARY);
	free(tmp);
	lws_callback_on_writable(wsi);
	return(ret);
}

/****************************************************************************************************************************/

static int callbackOgar(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	static unsigned int offset=0;
	static unsigned char rbuf[MAXLEN];

	switch (reason) {
	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		lwsl_notice("ogar: LWS_CALLBACK_CLIENT_ESTABLISHED\n");

		sendCommand(wsi,connectionStart1,sizeof(connectionStart1));
		sendCommand(wsi,connectionStart2,sizeof(connectionStart2));
		sendCommand(wsi,colorPackets[dogInfos.dogType],sizeOfColorPackets[dogInfos.dogType]);
		break;

 	case LWS_CALLBACK_CLIENT_WRITEABLE:
		if (writePacket(wsi) < 0 ) forceExit=1;
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		// we have receive some data, check if it can be written in static allocated buffer (length)

		if (offset + len < MAXLEN ) {
			memcpy(rbuf+offset,in,len);
			offset+=len;
			// we have receive some data, check with websocket API if this is a final fragment
			if (lws_is_final_fragment(wsi)) {
				// call recv function here
				messageReceived(wsi,rbuf,MAXLEN);
				offset=0;
			}
		} else {	// length is too long... get others but ignore them...
			offset=MAXLEN;
		 	if ( lws_is_final_fragment(wsi) ) {
				offset=0;
			}
		}
		break;
	case LWS_CALLBACK_CLOSED:
		lwsl_notice("ogar: LWS_CALLBACK_CLOSED\n");
		forceExit = 1;
		break;
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		lwsl_err("ogar: LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
		forceExit = 1;
		break;

	case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
		lwsl_err("ogar: LWS_CALLBACK_COMPLETED_CLIENT_HTTP\n");
		forceExit = 1;
		break;

	default:
		break;
	}


	return 0;
}

/****************************************************************************************************************************/
int main(int argc, char **argv)
{

	//Initialisation des variables
	dogInfos.dogType = 0;
	dogInfos.entity.positionX = 0;
	dogInfos.entity.positionY = 0;
	dogInfos.entity.ID = 0;
	dogInfos.entity.nickname = nicknames[dogInfos.dogType];
	dogInfos.targetPositionX = 1000;
	dogInfos.targetPositionY = 1000;

	numberOfEntity = 0;

	int n = 0;

	struct lws_context_creation_info info;
	struct lws_client_connect_info i;

	struct lws_context *context;
	const char *protocol,*temp;

	memset(&info, 0, sizeof info);
	memset(&i, 0, sizeof(i));

	if (argc < 2)
		goto usage;


	while (n >= 0) {
		n = getopt(argc, argv, "hsp:P:o:");
		if (n < 0)
			continue;
		switch (n) {
		case 's':
			i.ssl_connection = 2;
			break;
		case 'p':
			i.port = atoi(optarg);
			break;
		case 'o':
			i.origin = optarg;
			break;
		case 'P':
			info.http_proxy_address = optarg;
			break;
		case 'h':
			goto usage;
		}
	}

	if (optind >= argc)
		goto usage;

	signal(SIGINT, sighandler);

	if (lws_parse_uri(argv[optind], &protocol, &i.address, &i.port, &temp))
		goto usage;

	if (!strcmp(protocol, "http") || !strcmp(protocol, "ws"))
		i.ssl_connection = 0;
	if (!strcmp(protocol, "https") || !strcmp(protocol, "wss"))
		i.ssl_connection = 1;

	i.host = i.address;
	i.ietf_version_or_minus_one = -1;
	i.client_exts = NULL;
	i.path="/";

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	context = lws_create_context(&info);
	if (context == NULL) {
		fprintf(stderr, "Creating libwebsocket context failed\n");
		return 1;
	}

	i.context = context;

	if (lws_client_connect_via_info(&i)); // just to prevent warning !!

	forceExit=0;

	// the main magic here !!
	while (!forceExit) {
		lws_service(context, 1000);

	}
	// if there is some errors, we just quit
	lwsl_err("Exiting\n");
	lws_context_destroy(context);

	return 0;

usage:
	fprintf(stderr, "Usage: ogar-client -h -s -p <port> -P <proxy> -o <origin>  <server address> \n");
	return 1;
}
