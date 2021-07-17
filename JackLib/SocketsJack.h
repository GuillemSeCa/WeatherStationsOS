/*
* Mòdul amb els mètodes que treballen amb Sockets
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include "ConfigJack.h"
#include "SignalsJack.h"

//Constants
#define MAX_CONNECTIONS 20
#define MSG_JACK "$Jack:\n"
#define MSG_WAITING "Waiting..."
#define MSG_RECEIVING "Receiving data...\n"
#define MSG_ERR_SOCKET "Error durant la creacio del socket del Servidor Jack!\n"
#define MSG_ERR_BIND "Error durant el bind del port (Servidor Jack)!\n"
#define MSG_ERR_CREATE "Error al crear un nou thread (Servidor Jack)!\n"
#define MSG_ERR_ACCEPT "Error al crear acceptar la connexió d'un nou client al Servidor Jack!\n"

//Estructura per guardar les dades d'una estació
typedef struct {
    char *fileName;
    char *date;
    char *hour;
    float temperature;
    int humidity;
    float atmosphericPressure;
    float precipitation;
} Station;

//Estructura per guardar els paquets
typedef struct {
    char origen[14];
    char tipus;
    char dades[100];
} Packet;

//Mètode per configurar el servidor abans d'iniciar-lo
int launchServer(ConfigJack configJack);

//Mètode per iniciar el Servidor i esperar la connexió dels Clients Danny
void serverRun();

//Mètode que controlarà el comportament dels threads
void *connectionHandler(void *fdSocketServer);

//Mètode per tancar el servidor
void closeServer();