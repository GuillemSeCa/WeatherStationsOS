/*
* Mòdul amb els mètodes que treballen amb Sockets
*/
#pragma once

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "ConfigWendy.h"
#include "SignalsWendy.h"

//Constants
#define MAX_CONNECTIONS 20
#define MSG_WENDY "$Wendy:\n"
#define MSG_WAITING "Waiting...\n"
#define MSG_RECEIVING "Receiving data from "
#define MSG_ERR_SOCKET "Error durant la creacio del socket del Servidor Wendy!\n"
#define MSG_ERR_BIND "Error durant el bind del port (Servidor Wendy)!\n"
#define MSG_ERR_CREATE "Error al crear un nou thread (Servidor Wendy)!\n"
#define MSG_ERR_ACCEPT "Error al crear acceptar la connexió d'un nou client al Servidor Wendy!\n"

//Estructura per guardar els paquets
typedef struct
{
    char origen[14];
    char tipus;
    char dades[100];
} Packet;

//Estructura per guardar les dades d'una imatge
typedef struct
{
    char *fileName;
    int size;
    char *md5sum;
    char *data;
} Image;

//Mètode per calcular el MD5SUM d'un fitxer
void calculateMD5SUM(char md5sum[33], char imageFilePath[255]);

//Mètode per configurar el servidor abans d'iniciar-lo
int launchServer(ConfigWendy configWendy);

//Mètode per iniciar el Servidor i esperar la connexió dels Clients Danny
void serverRun();

//Mètode que controlarà el comportament dels threads
void *connectionHandler(void *fdSocketServer);

//Mètode per tancar el servidor
void closeServer();