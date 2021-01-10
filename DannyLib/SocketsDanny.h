/*
* Mòdul amb els mètodes que treballen amb Sockets
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include "ReadFilesDanny.h"

//Constants
#define MSG_ERR_SOCKET "Error durant la creacio del socket del Client!\n"
#define MSG_ERR_CONNECTION "Error en la connexió amb el servidor (Client)!\n"

//Mètode per connectar-se a un servidor a partir d'una IP i un Port
int connectWithServer(char *ip, int port);

//Mètode per tancar el File Director del Servidor
void closeConnectionServer();